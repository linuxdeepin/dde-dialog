#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDBusMetaType>

#include <DLog>

#include "DBusTypes.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setQuitOnLastWindowClosed(false);
    QGuiApplication app(argc, argv);

    Dtk::Core::DLogManager::registerConsoleAppender();

    qRegisterMetaType<Schema>("Schema");
    qDBusRegisterMetaType<Schema>();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qt/qml/agent/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
