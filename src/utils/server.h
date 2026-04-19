#pragma once
#include <QObject>
#include <QDBusContext>

class OrgKdeBuhoActionsInterface;

namespace AppInstance
{
inline constexpr qsizetype MaxNoteContentLength = 1 * 1024 * 1024;

QVector<QPair<QSharedPointer<OrgKdeBuhoActionsInterface>, QStringList>> appInstances(const QString& preferredService);

bool attachToExistingInstance(bool newNote = false, const QString &content = "");

bool registerService();
}

class Server : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.buho.Actions")

public:
    explicit Server(QObject *parent = nullptr);
    void setQmlObject(QObject  *object);

public Q_SLOTS:
    /**
           * Tries to raise/activate the Dolphin window.
           */
    void activateWindow();

    void newNote(const QString &content = "");


private:
    bool isTrustedCaller() const;
    QObject* m_qmlObject = nullptr;

};
