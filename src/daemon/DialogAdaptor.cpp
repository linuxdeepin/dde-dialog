#include "DialogAdaptor.h"

#include <QDBusConnectionInterface>
#include <QDebug>
#include <QUuid>

#include <yaml-cpp/yaml.h>
#include <fmt/format.h>
#include <systemd/sd-login.h>

#include <filesystem>

#include "config.h"
#include "Login1SessionInterface.h"
#include "AgentInterface.h"

const QString LOGIN1_SERVICE = "org.freedesktop.login1";
const QString LOGIN1_MANAGER_PATH = "/org/freedesktop/login1";

const QString AGENT_PATH = "/org/deepin/dde/dialog1/agent";

struct Data {
    std::vector<Schema> schemas;
};

const std::set<std::string> allowedActions = {
    "cancel",
    "ok",
};

namespace YAML {
template<>
struct convert<Data> {
  static bool decode(const Node &node, Data &rhs) {
    if(!node.IsMap()) {
        return false;
    }

    if (!node["schemas"]) {
        return false;
    }

    for (auto it = node["schemas"].begin(); it != node["schemas"].end(); ++it) {
        QStringList actions;
        auto actionsNode = (*it)["actions"];
        if (actionsNode.IsSequence()) {
            for (auto i = actionsNode.begin(); i != actionsNode.end(); ++i) {
                auto action = (*i).as<std::string>();
                if (allowedActions.find(action) == allowedActions.end()) {
                    if (action.rfind("custom:", 0) != 0) {
                        throw Exception(i->Mark(), fmt::format("invalid action {}", action));
                    }
                }

                actions << QString::fromStdString(action);
            }
        }

        auto getString = [](const Node &n) -> std::string {
            return (n && !n.IsNull()) ? n.as<std::string>() : std::string();
        };

        rhs.schemas.push_back({
            QString::fromStdString(getString((*it)["id"])),
            QString::fromStdString(getString((*it)["type"])),
            QString::fromStdString(getString((*it)["gettext-domain"])),
            QString::fromStdString(getString((*it)["description"])),
            QString::fromStdString(getString((*it)["message"])),
            actions,
        });
    }

    return true;
  }
};
}

DialogAdaptor::DialogAdaptor(const QDBusConnection &bus, QObject *parent)
    : QObject(parent)
    , m_bus(bus)
    , m_login1Manager(LOGIN1_SERVICE, LOGIN1_MANAGER_PATH, m_bus) {
    m_serviceWatcher.setConnection(m_bus);
    connect(&m_serviceWatcher,
            &QDBusServiceWatcher::serviceOwnerChanged,
            this,
            &DialogAdaptor::UnregisterAgentByService);

    std::filesystem::directory_iterator it(DATA_DIR);
    for (auto const &entry : std::filesystem::directory_iterator{DATA_DIR}) {
        if (!entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".yaml") {
            continue;
        }

        qInfo() << "loading" << entry.path().string();
        try {
            auto doc = YAML::LoadFile(entry.path()).as<Data>();
            m_schemas.insert(m_schemas.end(), doc.schemas.begin(), doc.schemas.end());
        } catch (const YAML::ParserException &e) {
            qWarning() << "failed to parse" << e.what();
        } catch (const YAML::BadFile &e) {
            qWarning() << "failed to open" << e.what();
        } catch (const YAML::InvalidNode &e) {
            qWarning() << "invalid node" << e.what();
        } catch (const YAML::Exception &e) {
            qWarning() << e.what();
        }
    }
}

DialogAdaptor::~DialogAdaptor() = default;

void DialogAdaptor::RegisterAgent(const QDBusMessage &message) {
    QString session = getSessionByService(message.service());
    if (session.isEmpty()) {
        // todo: log
        m_bus.send(message.createErrorReply(QDBusError::Failed, "No session found"));
        return;
    }

    if (m_agents.contains(session)) {
        m_bus.send(message.createErrorReply(QDBusError::AccessDenied, "Agent already registered"));
        return;
    }

    qInfo("agent %s of session %s registered", message.service().toStdString().c_str(), session.toStdString().c_str());
    m_serviceWatcher.addWatchedService(message.service());
    m_agents[session] = message.service();
}

void DialogAdaptor::UnregisterAgent(const QDBusMessage &message) {
    UnregisterAgentByService(message.service());
}

ActionType DialogAdaptor::Open(const QString &actionId, const QVariantMap &options, const QString &cancelationId, const QDBusMessage &message) {
    auto it = std::find_if(m_schemas.begin(), m_schemas.end(), [&actionId](const auto &action) {
        return action.id == actionId;
    });

    if (it == m_schemas.end()) {
        m_bus.send(message.createErrorReply(QDBusError::Failed, "Action not found"));
        return ActionType{};
    }

    if (!cancelationId.isEmpty() && m_cancelationMap.contains(cancelationId)) {
        m_bus.send(message.createErrorReply(QDBusError::AccessDenied, "Cancelation already registered"));
        return ActionType{};
    }

    auto reply = m_login1Manager.ListSessions();
    reply.waitForFinished();
    if (reply.isError()) {
        m_bus.send(message.createErrorReply(QDBusError::Failed, "Failed to get sessions"));
        return ActionType{};
    }

    QString activatedSession;
    auto sessions = reply.value();
    for (auto &session : sessions) {
        if (!m_agents.contains(session.id)) {
            continue;
        }

        org::freedesktop::login1::Session s(LOGIN1_SERVICE, session.objectPath.path(), m_bus);
        if (s.active()) {
            activatedSession = session.id;
            break;
        }
    }

    if (activatedSession.isEmpty()) {
        m_bus.send(message.createErrorReply(QDBusError::Failed, "No activated agent"));
        return ActionType{};
    }

    auto cookie = QUuid::createUuid().toString(QUuid::WithoutBraces);

    auto agentService = m_agents[activatedSession];
    org::deepin::dde::dialog1::agent agent(agentService, AGENT_PATH, m_bus);
    auto openReply = agent.Open(cookie, *it, options);
    openReply.waitForFinished();
    if (openReply.isError()) {
        auto err = openReply.error();
        m_bus.send(message.createErrorReply(err));
        return ActionType{};
    }

    message.setDelayedReply(true);
    if (!cancelationId.isEmpty()) {
        m_cancelationMap.emplace(cancelationId, agentService, cookie);
    }
    m_messageMap.emplace(cookie, message);
    return ActionType{};
}

void DialogAdaptor::Cancel(const QString &cancelationId, const QDBusMessage &message) {
    auto it = m_cancelationMap.find(cancelationId);
    if (it == m_cancelationMap.end()) {
        m_bus.send(message.createErrorReply(QDBusError::Failed, "cancelation not found"));
        return;
    };

    auto agentService = it.value().first;
    auto cookie = it.value().second;

    org::deepin::dde::dialog1::agent agent(agentService, AGENT_PATH, m_bus);
    auto reply = agent.Cancel(cookie);
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        m_bus.send(message.createErrorReply(err));
        return;
    }

    m_cancelationMap.erase(it);
    m_messageMap.remove(cookie);
}

void DialogAdaptor::AgentResponse(const QString &cookie, ActionType action, const QDBusMessage &message) {
    auto it = m_messageMap.find(cookie);
    if (it == m_messageMap.end()) {
        m_bus.send(message.createErrorReply(QDBusError::Failed, "cookie not found"));
        return;
    }

    auto msg = it.value();
    m_messageMap.erase(it);

    m_bus.send(msg.createReply(action));
}

QString DialogAdaptor::getSessionByService(const QString &service) {
    auto pid = m_bus.interface()->servicePid(service);
    if (!pid.isValid()) {
        // todo: log
        return "";
    }

    char *session = nullptr;
    int res = sd_pid_get_session(pid, &session);
    if (res >= 0) {
        QString result = QString::fromUtf8(session);
        free(session);
        return result;
    }

    uid_t uid = 0;
    res = sd_pid_get_owner_uid(pid, &uid);
    if (res >= 0) {
        res = sd_uid_get_display(uid, &session);
        if (res >= 0) {
            QString result = QString::fromUtf8(session);
            free(session);
            return result;
        }
    }

    return "";
}

void DialogAdaptor::UnregisterAgentByService(const QString &service) {
    qInfo() << "UnregisterAgentByService" << service;
    auto it = std::find_if(m_agents.begin(), m_agents.end(), [&service] (const auto &v) {
        return v == service;
    });
    if (it == m_agents.end()) {
        return;
    }

    qInfo("agent %s of session %s unregistered", service.toStdString().c_str(), it.key().toStdString().c_str());
    m_agents.erase(it);
}
