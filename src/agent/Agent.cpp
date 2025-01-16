#include "Agent.h"

const QString DIALOG_SERVICE = "org.deepin.dde.dialog1";
const QString DIALOG_PATH = "/org/deepin/dde/dialog1";

const QString AGENT_PATH = "/org/deepin/dde/dialog1/agent";

Agent::Agent()
    : m_bus(QDBusConnection::systemBus())
    , m_adaptor(m_bus, &m_model, this)
    , m_daemon(DIALOG_SERVICE, DIALOG_PATH, m_bus) {
    if (!m_bus.registerObject(AGENT_PATH, &m_adaptor, QDBusConnection::ExportScriptableContents)) {
        throw std::runtime_error("failed to register object");
    }

    auto reply = m_daemon.RegisterAgent();
    reply.waitForFinished();
    if (reply.isError()) {
        throw std::runtime_error("failed to register agent: " + reply.error().message().toStdString());
    }

    connect(&m_model, &DialogModel::triggered, this, &Agent::onTriggered);
}

Agent::~Agent() {
    m_bus.unregisterObject(AGENT_PATH);
}

DialogModel *Agent::model() {
    return &m_model;
}

void Agent::onTriggered(const QString &cookie, int action) {
    m_daemon.AgentResponse(cookie, action);
}
