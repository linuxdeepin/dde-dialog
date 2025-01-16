#pragma once

#include <QObject>
#include <QDBusConnection>
#include <QDBusArgument>

#include "DBusTypes.h"

class DialogModel;

class AgentAdaptor : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.dialog1.agent")

public:
    AgentAdaptor(const QDBusConnection &bus, DialogModel *model, QObject *parent = nullptr);
    ~AgentAdaptor();

// dbus methods
public Q_SLOTS:
    Q_SCRIPTABLE void Open(const QString &cookie, const Schema &schema, const QVariantMap &options, const QDBusMessage &message);
    Q_SCRIPTABLE void Cancel(const QString &cookie);

private:
    QDBusConnection m_bus;
    DialogModel *m_model;
};
