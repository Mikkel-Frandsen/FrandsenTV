//
// Created by LHF on 31/08/2026.
//
#include <QDebug>


#include "CoreClient.h"
CoreClient::CoreClient() {

    //We want it to let us know once it is connected, and thus setup a signal here.
    QObject::connect(
    &socket,
    &QLocalSocket::connected,
    [this]() {
        qDebug() << "Connected!";

    });

    QObject::connect(
        &socket,
        &QLocalSocket::readyRead,
        [this]() {
            qDebug() << "Message received!";
            readMessage();
        }
        );



}

void CoreClient::connectToCore() {
    socket.connectToServer("smartTV-core");
}

void CoreClient::requestStatus() {
    socket.write("STATUS");
}

void CoreClient::readMessage() {
    QByteArray message = socket.readAll();
    if (message == "ONLINE") {
        qDebug() << "Recieved ONLINE status";
        emit statusChanged(true);
    }
}


