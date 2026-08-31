#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocalSocket>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QLocalSocket socket;


    //Once we get this signal QLocalSocket::connected()
    //We should print connected!
    QObject::connect(
    &socket,
    &QLocalSocket::connected,
    []() {
        qDebug() << "Connected!";
    }
);

    socket.connectToServer("smartTV-core");

    return app.exec();

}
