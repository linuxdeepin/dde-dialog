#pragma once

#include <QObject>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QFileSystemWatcher>
#include <QDBusArgument>

#include "Login1ManagerInterface.h"

class DialogAdaptor : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.dialog1");

public:
    DialogAdaptor(const QDBusConnection &bus, QObject *parent = nullptr);
    ~DialogAdaptor();

public slots:
    Q_SCRIPTABLE void RegisterAgent(const QDBusMessage &message);
    Q_SCRIPTABLE void UnregisterAgent(const QDBusMessage &message);
    Q_SCRIPTABLE ActionType Open(const QString &actionId, const QVariantMap &options, const QString &cancelationId, const QDBusMessage &message);
    Q_SCRIPTABLE void Cancel(const QString &cancelationId, const QDBusMessage &message);
    Q_SCRIPTABLE void AgentResponse(const QString &cookie, int actionIdx, const QDBusMessage &message);

private:
    QString getSessionByService(const QString &service);

private slots:
    void UnregisterAgentByService(const QString &service);

private:
    QDBusConnection m_bus;
    std::vector<Schema> m_schemas;
    QHash<QString, QString> m_agents;
    QDBusServiceWatcher m_serviceWatcher;

    org::freedesktop::login1::Manager m_login1Manager;
    QHash<QString, std::pair<QString, QString>> m_cancelationMap;
    QHash<QString, QDBusMessage> m_messageMap;
};
