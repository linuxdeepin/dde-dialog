#include "AgentAdaptor.h"

#include <DialogModel.h>

AgentAdaptor::AgentAdaptor(const QDBusConnection &bus, DialogModel *model, QObject *parent)
    : QObject(parent)
    , m_bus(bus)
    , m_model(model) {
}

AgentAdaptor::~AgentAdaptor() = default;

void AgentAdaptor::Open(const QString &cookie, const Schema &schema, const QVariantMap &options, const QDBusMessage &message) {
    qInfo() << "Open" << schema.id;

    auto *di = new DialogInstance(cookie, schema, options, this);
    m_model->newDialog(di, options);
}

void AgentAdaptor::Cancel(const QString &cookie) {

}
