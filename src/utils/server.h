#pragma once
#include <QObject>

class OrgKdeBuhoActionsInterface;

namespace AppInstance
{
QVector<QPair<QSharedPointer<OrgKdeBuhoActionsInterface>, QStringList>> appInstances(const QString& preferredService);

bool attachToExistingInstance(bool newNote = false, const QString &content = "");

bool registerService();
}

class Server : public QObject
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

    /** Stores all settings and quits Dolphin. */
    void quit();

    void newNote(const QString &content = "");


private:
    QObject* m_qmlObject = nullptr;

};

