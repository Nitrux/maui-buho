#include "notescontroller.h"
#include "db/db.h"
#include "owl.h"

#include <QDir>
#include <QSqlQuery>
#include <MauiKit4/FileBrowsing/fmstatic.h>
#include <MauiKit4/FileBrowsing/tagging.h>

Q_DECLARE_METATYPE(FMH::MODEL_LIST)
Q_DECLARE_METATYPE(FMH::MODEL)

NotesController::NotesController(QObject *parent)
    : QObject(parent)
    , m_db(DB::getInstance())
{
    qRegisterMetaType<FMH::MODEL_LIST>("MODEL_LIST");
    qRegisterMetaType<FMH::MODEL>("MODEL");

    auto m_loader = new NotesLoader;
    m_loader->moveToThread(&m_worker);

    connect(&m_worker, &QThread::finished, m_loader, &QObject::deleteLater);
    connect(this, &NotesController::fetchNotes, m_loader, &NotesLoader::fetchNotes);

    connect(m_loader, &NotesLoader::noteReady, this, &NotesController::noteReady);
    connect(m_loader, &NotesLoader::notesReady, this, &NotesController::notesReady);

    m_worker.start();
}

NotesController::~NotesController()
{
    m_worker.quit();
    m_worker.wait();
}

bool NotesController::insertNote(FMH::MODEL &note)
{
    if (note.isEmpty()) {
        qWarning() << "Could not insert note locally. The note is empty. NotesController::insertNote.";
        return false;
    }

    if ((OWL::NotesPath.isLocalFile() && !FMH::fileExists(OWL::NotesPath)) || OWL::NotesPath.isEmpty() || !OWL::NotesPath.isValid()) {
        qWarning() << "The url destination is not valid or does not exists, therefore it could not be saved into a file" << OWL::NotesPath;
        qWarning() << "File could not be saved. NotesController::insertNote.";
        return false;
    }

    note[FMH::MODEL_KEY::ID] = OWL::createId();
    const auto url_ = QUrl(OWL::NotesPath.toString() + note[FMH::MODEL_KEY::ID] + note[FMH::MODEL_KEY::FORMAT]);
    const QString expectedBase = QDir::cleanPath(OWL::NotesPath.toLocalFile());
    const QString notePath = QDir::cleanPath(url_.toLocalFile());
    if (!notePath.startsWith(expectedBase + "/")) {
        qWarning() << "Rejected note path outside notes directory:" << notePath;
        return false;
    }
    if (!OWL::saveNoteFile(url_, note[FMH::MODEL_KEY::CONTENT].toUtf8()))
        return false;

    note[FMH::MODEL_KEY::URL] = url_.toString();

    //	for(const auto &tg : note[FMH::MODEL_KEY::TAG].split(",", QString::SplitBehavior::SkipEmptyParts))
    //        Tagging::getInstance()->tagAbstract(tg, "notes", note[FMH::MODEL_KEY::URL], note[FMH::MODEL_KEY::COLOR]);

    return (this->m_db->insert(OWL::TABLEMAP[OWL::TABLE::NOTES], FMH::toMap(FMH::filterModel(note, {FMH::MODEL_KEY::URL, FMH::MODEL_KEY::ID, FMH::MODEL_KEY::COLOR, FMH::MODEL_KEY::FAVORITE}))));
}

bool NotesController::updateNote(FMH::MODEL &note, QString id)
{
    if (note.isEmpty())
        return false;

    if (note[FMH::MODEL_KEY::URL].isEmpty())
        note[FMH::MODEL_KEY::URL] = [&]() -> const QString {
            QSqlQuery query(DB::getInstance()->db());
            query.prepare(QStringLiteral("SELECT url FROM notes WHERE id = ?"));
            query.addBindValue(id);
            if (query.exec() && query.next())
                return query.value(0).toString();
            return QString();
        }();

    if (note[FMH::MODEL_KEY::URL].isEmpty())
        return false;

    const auto f_note = FMH::toMap(FMH::filterModel(note, {FMH::MODEL_KEY::COLOR, FMH::MODEL_KEY::FAVORITE}));
    if (f_note.isEmpty())
        return true;

    return this->m_db->update(OWL::TABLEMAP[OWL::TABLE::NOTES], f_note, QVariantMap{{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});
}

bool NotesController::removeNote(const QString &id)
{
    const auto url = QUrl([&]() -> const QString {
        QSqlQuery query(DB::getInstance()->db());
        query.prepare(QStringLiteral("SELECT url FROM notes WHERE id = ?"));
        query.addBindValue(id);
        if (query.exec() && query.next())
            return query.value(0).toString();
        return QString();
    }());

    this->m_db->remove(OWL::TABLEMAP[OWL::TABLE::NOTES_SYNC], {{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});

    FMStatic::removeFiles({url});

    return this->m_db->remove(OWL::TABLEMAP[OWL::TABLE::NOTES], {{FMH::MODEL_NAME[FMH::MODEL_KEY::ID], id}});
}

void NotesController::getNotes()
{
    Q_EMIT this->fetchNotes(this->m_db->getDBData("select * from notes"));
}

void NotesLoader::fetchNotes(FMH::MODEL_LIST notes)
{
    for (auto &note : notes) {
        const auto url = QUrl(note[FMH::MODEL_KEY::URL]);
        const auto contentPreview = OWL::fileContentPreview(url);
        note[FMH::MODEL_KEY::CONTENT] = contentPreview;
        Q_EMIT this->noteReady(note);
    }

    qDebug() << "FINISHED FETCHING URLS";
    Q_EMIT this->notesReady(notes);
}
