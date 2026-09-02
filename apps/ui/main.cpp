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


    core_client.connectToServer("smartTV-core");
    core_client.ping(); //This essentially just shows that on the UI we have not connected before we connect ;=)

    return app.exec();

}
