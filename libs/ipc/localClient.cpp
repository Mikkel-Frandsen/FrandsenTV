//
// Created by LHF on 02/09/2026.
//

#include "localClient.h"
#include <QDebug>
LocalClient::LocalClient() {

    //We want it to let us know once it is connected, and thus setup a signal here.
    QObject::connect(
    &socket,
    &QLocalSocket::connected,
    []() {
        qDebug() << "Connected!";
    });

    QObject::connect(
        &socket,
        &QLocalSocket::readyRead,
        this,
        &LocalClient::readMessage
        );



}

void LocalClient::connectToCore() {
    socket.connectToServer("smartTV-core");
}



// ----- Sending of messages (Queries/Commands) -----

//Centrilized function to send messages. Ensures we are connected to the server
void LocalClient::sendMessage(QByteArrayView message) {
    if (socket.state() != QLocalSocket::ConnectedState) {
        qWarning() << "Cannot send message: not connected";
        return;
    }
    socket.write(QByteArray(message));
    socket.flush();
}




void LocalClient::readMessage() {
    readBuffer += socket.readAll();
    while (readBuffer.contains('\n')) {
        //Find the first \n
        const int newlineIndex = readBuffer.indexOf('\n');
        //Take all bytes before that \n
        QByteArray message = readBuffer.left(newlineIndex + 1);
        //Remove that message from the buffer.
        readBuffer.remove(0, newlineIndex + 1);
        handleMessage(message, &socket);

    }
}


