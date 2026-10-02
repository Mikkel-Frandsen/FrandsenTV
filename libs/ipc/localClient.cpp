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




void LocalClient::readMessage()
{


    const QByteArray received = socket.readAll();


    if (received.isEmpty())
        return;

    readBuffer.append(received);

    while (true) {
        //There was a bug here, after I added 11 enums! Because now
        //it became a \n...
        const qsizetype newlineIndex =
            readBuffer.indexOf('\n', sizeof(int));

        if (newlineIndex == -1) {
            return;
        }

        QByteArray messageBytes =
            readBuffer.left(newlineIndex);

        readBuffer.remove(0, newlineIndex + 1);


        if (messageBytes.isEmpty())
            continue;

        Protocol::Message message =
            Protocol::decode(messageBytes);

        handleMessage(message);
    }
}