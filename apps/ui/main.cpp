#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocalSocket>
#include <QDebug>

#include "CoreClient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    CoreClient core_client;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        "coreClient",
        &core_client
    );
    engine.loadFromModule("SmartTV", "Main");


    core_client.connectToCore();


    return app.exec();

}
