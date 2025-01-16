#include "DBusTypes.h"

QDBusArgument &operator<<(QDBusArgument &argument, const ListSessionsSession &listSessionSession)
{
    argument.beginStructure();
    argument << listSessionSession.id;
    argument << listSessionSession.userId;
    argument << listSessionSession.username;
    argument << listSessionSession.seatId;
    argument << listSessionSession.objectPath;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ListSessionsSession &listSessionSession)
{
    argument.beginStructure();
    argument >> listSessionSession.id;
    argument >> listSessionSession.userId;
    argument >> listSessionSession.username;
    argument >> listSessionSession.seatId;
    argument >> listSessionSession.objectPath;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Schema &schema)
{
    argument.beginStructure();
    argument << schema.id;
    argument << schema.type;
    argument << schema.gettextDomain;
    argument << schema.description;
    argument << schema.message;
    argument << schema.actions;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Schema &schema)
{
    argument.beginStructure();
    argument >> schema.id;
    argument >> schema.type;
    argument >> schema.gettextDomain;
    argument >> schema.description;
    argument >> schema.message;
    argument >> schema.actions;
    argument.endStructure();

    return argument;
}
