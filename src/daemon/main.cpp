#include "DialogAdaptor.h"

#include <QCoreApplication>
#include <QDBusConnection>

#include "DBusTypes.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qRegisterMetaType<ListSessionsSession>("ListSessionsSession");
    qDBusRegisterMetaType<ListSessionsSession>();
    qRegisterMetaType<QVector<ListSessionsSession>>("QVector<ListSessionsSession>");
    qDBusRegisterMetaType<QVector<ListSessionsSession>>();
    qRegisterMetaType<Schema>("Schema");
    qDBusRegisterMetaType<Schema>();

    auto bus = QDBusConnection::systemBus();
    if (!bus.registerService("org.deepin.dde.dialog1")) {
        qFatal("Failed to register service");
        return 1;
    }

    if (!bus.registerObject("/org/deepin/dde/dialog1", new DialogAdaptor(bus), QDBusConnection::ExportScriptableContents)) {
        qFatal("Failed to register object");
        return 1;
    }

    return a.exec();
}
