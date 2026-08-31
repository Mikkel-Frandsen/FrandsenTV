#include<QCoreApplication>
#include <QLocalServer>
#include <QDebug>

#include "CoreServer.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    CoreServer core_server;

    qDebug() << "Core online:" << core_server.isOnline();
    return app.exec();
}

