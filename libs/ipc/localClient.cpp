//
// Created by LHF on 02/09/2026.
//

#include "localClient.h"
#include <QDebug>

LocalClient::LocalClient() {

    //We want it to let us know once it is connected, and thus setup a signal here.
    connect(
    &socket,
    &QLocalSocket::connected,
    []() {
        qDebug() << "Connected!";
    });

    connect(
        &socket,
        &QLocalSocket::readyRead,
        this,
        &LocalClient::readMessage
        );



}

void LocalClient::connectToServer(const QString& serverName) {
    socket.connectToServer(serverName);
}



// ----- Sending of messages (Queries/Commands) -----

//Centrilized function to send messages. Ensures we are connected to the server
void LocalClient::sendMessage(Protocol::Message message) {
    if (socket.state() != QLocalSocket::ConnectedState) {
        qWarning() << "Cannot send message: not connected";
        return;
    }
    socket.write(Protocol::encode((message)));
    socket.flush();
}




void LocalClient::readMessage() {
    readBuffer += socket.readAll();

    while (readBuffer.contains('\n')) {
        //Find the first \n
        const int newlineIndex = readBuffer.indexOf('\n');
        //Take all bytes before that \n
        QByteArray message_bytes = readBuffer.left(newlineIndex + 1);
        //Remove that message from the buffer.
        readBuffer.remove(0, newlineIndex + 1);
        Protocol::Message message = Protocol::decode(message_bytes);
        handleMessage(message);
    }
}


