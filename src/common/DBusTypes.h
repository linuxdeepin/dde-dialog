#pragma once

#include <QString>
#include <QDBusArgument>
#include <QDBusObjectPath>

// #include "EnumDBus.h"

struct ListSessionsSession {
    QString id;
    uint32_t userId;
    QString username;
    QString seatId;
    QDBusObjectPath objectPath;
};
Q_DECLARE_METATYPE(ListSessionsSession)
Q_DECLARE_METATYPE(QVector<ListSessionsSession>)

QDBusArgument &operator<<(QDBusArgument &argument, const ListSessionsSession &listSessionSession);
const QDBusArgument &operator>>(const QDBusArgument &argument, ListSessionsSession &listSessionSession);

using ActionType = int;
inline ActionType ActionTypeCancel = 0;
inline ActionType ActionTypeOk = 1;
inline ActionType ActionTypeCustom = 2;

struct Schema {
    QString id;
    QString type;
    QString gettextDomain;
    QString description;
    QString message;
    QStringList actions;
};
Q_DECLARE_METATYPE(Schema)

QDBusArgument &operator<<(QDBusArgument &argument, const Schema &action);
const QDBusArgument &operator>>(const QDBusArgument &argument, Schema &action);
