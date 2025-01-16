#pragma once

#include <QObject>

#include <fmt/args.h>

#include "DBusTypes.h"

class DialogInstance : public QObject {
    Q_OBJECT

public:
    DialogInstance(const QString &cookie, const Schema &schema, const QVariantMap &options, QObject *parent);
    ~DialogInstance();

private:
    fmt::dynamic_format_arg_store<fmt::format_context> getFieldsStore(const QList<QVariant> &fields);

public:
    QString cookie;
    QString type;
    QString description;
    QString message;
    QStringList actions;
};
