#include "server.h"

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QDBusReply>
#include <QFileInfo>
#include <QRegularExpression>

#include <MauiKit4/FileBrowsing/fmstatic.h>

#include "buhointerface.h"
#include "buhoadaptor.h"

namespace
{
QString canonicalPath(const QString &path)
{
    const QFileInfo info(path);
    const auto canonical = info.canonicalFilePath();
    return canonical.isEmpty() ? info.absoluteFilePath() : canonical;
}

bool isExpectedServiceName(const QString &service)
{
    static const QRegularExpression pattern(QStringLiteral("^org\\.kde\\.buho-[0-9]+$"));
    return pattern.match(service).hasMatch();
}

QString serviceExecutablePath(const QString &service)
{
    if (!isExpectedServiceName(service))
        return {};

    auto *sessionInterface = QDBusConnection::sessionBus().interface();
    if (!sessionInterface)
        return {};

    const auto owner = sessionInterface->serviceOwner(service);
    if (!owner.isValid() || owner.value().isEmpty())
        return {};

    const auto pid = sessionInterface->servicePid(owner.value());
    if (!pid.isValid() || pid.value() == 0)
        return {};

    return canonicalPath(QStringLiteral("/proc/%1/exe").arg(pid.value()));
}

bool isTrustedBuhoService(const QString &service)
{
    const auto ownerPath = serviceExecutablePath(service);
    if (ownerPath.isEmpty())
        return false;

    return ownerPath == canonicalPath(QCoreApplication::applicationFilePath());
}
}

QVector<QPair<QSharedPointer<OrgKdeBuhoActionsInterface>, QStringList>> AppInstance::appInstances(const QString& preferredService)
{
    QVector<QPair<QSharedPointer<OrgKdeBuhoActionsInterface>, QStringList>> dolphinInterfaces;

    if (!preferredService.isEmpty())
    {
        QSharedPointer<OrgKdeBuhoActionsInterface> preferredInterface(
                    new OrgKdeBuhoActionsInterface(preferredService,
                                                   QStringLiteral("/Actions"),
                                                   QDBusConnection::sessionBus()));

        if (isTrustedBuhoService(preferredService) && preferredInterface->isValid() && !preferredInterface->lastError().isValid()) {
            dolphinInterfaces.append(qMakePair(preferredInterface, QStringList()));
        }
    }

    // Look for dolphin instances among all available dbus services.
    QDBusConnectionInterface *sessionInterface = QDBusConnection::sessionBus().interface();
    const QStringList dbusServices = sessionInterface ? sessionInterface->registeredServiceNames().value() : QStringList();
    // Don't match the service without trailing "-" (unique instance)
    const QString pattern = QStringLiteral("org.kde.buho-");

    // Don't match the pid without leading "-"
    const QString myPid = QLatin1Char('-') + QString::number(QCoreApplication::applicationPid());

    for (const QString& service : dbusServices)
    {
        if (service.startsWith(pattern) && !service.endsWith(myPid) && isTrustedBuhoService(service))
        {
            QSharedPointer<OrgKdeBuhoActionsInterface> interface(
                        new OrgKdeBuhoActionsInterface(service,
                                                       QStringLiteral("/Actions"),
                                                       QDBusConnection::sessionBus()));
            if (interface->isValid() && !interface->lastError().isValid())
            {
                dolphinInterfaces.append(qMakePair(interface, QStringList()));
            }
        }
    }

    return dolphinInterfaces;
}

bool AppInstance::attachToExistingInstance(bool newNote, const QString& content)
{
    bool attached = false;

    auto dolphinInterfaces = appInstances("");
    if (dolphinInterfaces.isEmpty())
    {
        return attached;
    }

    for (const auto& interface: std::as_const(dolphinInterfaces))
    {
        if(newNote)
        {
            auto reply = interface.first->newNote(content);
            reply.waitForFinished();

            if (!reply.isError())
            {
                interface.first->activateWindow();
                attached = true;
                break;
            }
        }else
        {
            auto reply = interface.first->activateWindow();
            reply.waitForFinished();

            if (!reply.isError())
            {
                attached = true;
                break;
            }
        }
    }

    return attached;
}

bool AppInstance::registerService()
{
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();

    auto registration = iface->registerService(QStringLiteral("org.kde.buho-%1").arg(QCoreApplication::applicationPid()),
                                               QDBusConnectionInterface::DontQueueService,
                                               QDBusConnectionInterface::DontAllowReplacement);

    if (!registration.isValid())
    {
        qWarning("2 Failed to register D-Bus service \"%s\" on session bus: \"%s\"",
                 qPrintable("org.kde.buho"),
                 qPrintable(registration.error().message()));
        return false;
    }

    if (registration.value() == QDBusConnectionInterface::ServiceNotRegistered)
    {
        qWarning("D-Bus service \"%s\" is already registered by another instance.",
                 qPrintable(QStringLiteral("org.kde.buho-%1").arg(QCoreApplication::applicationPid())));
        return false;
    }

    return true;
}

Server::Server(QObject *parent) : QObject(parent)
  , m_qmlObject(nullptr)
{
    new ActionsAdaptor(this);
    if(!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Actions"), this))
    {
        qDebug() << "FAILED TO REGISTER BACKGROUND DBUS OBJECT";
        return;
    }
}

void Server::setQmlObject(QObject *object)
{
    if(!m_qmlObject)
    {
        m_qmlObject = object;
    }
}

void Server::activateWindow()
{
    if (!this->isTrustedCaller())
    {
        qWarning() << "Rejected untrusted D-Bus request to activate Buho";
        return;
    }

    if(m_qmlObject)
    {
        auto window = qobject_cast<QQuickWindow *>(m_qmlObject);
        if (window)
        {
            window->raise();
            window->requestActivate();
        }
    }
}

void Server::newNote(const QString &content)
{
    if (!this->isTrustedCaller())
    {
        qWarning() << "Rejected untrusted D-Bus request to create a note";
        return;
    }

    if(m_qmlObject)
    {
        QMetaObject::invokeMethod(m_qmlObject, "newNote",
                                  Q_ARG(QString, content.left(AppInstance::MaxNoteContentLength)));

    }
}

bool Server::isTrustedCaller() const
{
    if (!this->calledFromDBus())
        return true;

    return isTrustedBuhoService(this->message().service());
}
