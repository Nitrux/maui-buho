#include <QCommandLineParser>
#include <QDate>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

#include <KLocalizedString>

#include <MauiKit4/Core/mauiapp.h>
#include <MauiKit4/TextEditor/moduleinfo.h>

#include <QGuiApplication>
#include <QSurfaceFormat>

#include "../buho_version.h"

#include "owl.h"
#include "models/notes/notes.h"
#include "utils/server.h"

#define BUHO_URI "org.maui.buho"

/**
 * @brief setFolders Creates the directory where to save the note as text files
 */
static void setFolders()
{
    QDir notes_path(OWL::NotesPath.toLocalFile());
    if (!notes_path.exists())
        notes_path.mkpath(".");
}

int Q_DECL_EXPORT main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

    setFolders ();

    app.setOrganizationName(QStringLiteral("Maui"));
    app.setWindowIcon(QIcon(":/buho.png"));

    KLocalizedString::setApplicationDomain("buho");
    KAboutData about(QStringLiteral("buho"),
                     i18n("Buho"),
                     BUHO_VERSION_STRING,
                     i18n("Create and organize your notes."),
                     KAboutLicense::LGPL_V3,
                     i18n("© %1 Made by Nitrux | Built with MauiKit", QString::number(QDate::currentDate().year())),
                     QString(GIT_BRANCH) + "/" + QString(GIT_COMMIT_HASH));

    about.addAuthor(QStringLiteral("Camilo Higuita"), i18n("Developer"), QStringLiteral("milo.h@aol.com"));
    about.addAuthor(QStringLiteral("Uri Herrera"), i18n("Developer"), QStringLiteral("uri_herrera@nxos.org"));
    about.setHomepage("https://nxos.org");
    about.setProductName("nitrux/buho");
    about.setOrganizationDomain(BUHO_URI);
    about.setDesktopFileName("org.maui.buho");
    about.setProgramLogo(app.windowIcon());

    const auto FBData = MauiKitTextEditor::aboutData();
    about.addComponent(FBData.name(), MauiKitTextEditor::buildVersion(), FBData.version(), FBData.webAddress());

    KAboutData::setApplicationData(about);
    MauiApp::instance()->setIconName("qrc:/buho.svg");

    QCommandLineOption newNoteOption(QStringList() << "n" << "new", "Create a new note.");
    QCommandLineOption newNoteContent(QStringList() << "c" << "content", "new note contents.", "content");

    QCommandLineParser parser;

    parser.addOption(newNoteOption);
    parser.addOption(newNoteContent);

    about.setupCommandLine(&parser);
    parser.process(app);

    about.processCommandLine(&parser);

    bool newNote = parser.isSet(newNoteOption);
    QString noteContent;

    if(newNote)
    {
        if(parser.isSet(newNoteContent))
        {
            noteContent = parser.value(newNoteContent).left(1 * 1024 * 1024); // cap at 1 MB
        }
    }

    if (AppInstance::attachToExistingInstance(newNote, noteContent))
    {
        // Successfully attached to existing instance of Nota
        return 0;
    }

    AppInstance::registerService();

    auto server = std::make_unique<Server>();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/app/maui/buho/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url, newNote, noteContent, &server](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);

            server->setQmlObject(obj);
            if(newNote)
            {
                server->newNote(noteContent);
            }
        },
        Qt::QueuedConnection);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    qmlRegisterType<Notes>(BUHO_URI, 1, 0, "Notes");

    engine.load(url);
    return app.exec();
}
