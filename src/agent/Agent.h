#pragma once

#include "QObject"
#include <QQmlEngine>
#include <QDBusConnection>

#include "AgentAdaptor.h"
#include "DialogInterface.h"
#include "DialogModel.h"

class Agent : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    Agent();
    ~Agent();

public:
    Q_PROPERTY(DialogModel* model READ model CONSTANT)
    DialogModel *model();

private Q_SLOTS:
    void onTriggered(const QString &cookie, int action);

private:
    QDBusConnection m_bus;
    DialogModel m_model;
    AgentAdaptor m_adaptor;
    org::deepin::dde::dialog1 m_daemon;
};
