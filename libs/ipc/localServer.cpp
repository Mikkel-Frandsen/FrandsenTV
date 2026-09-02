//
// Created by LHF on 02/09/2026.
//

#include "localServer.h"
LocalServer::LocalServer()
{
    if (!server.listen("smartTV-core")) {
        qWarning() << server.errorString();
    }
    //This creates a signal for the server, that fires once a newConnection is achived.
    //Once a new connection is found, we simply call the function newConnection (Using "this" instance)
    QObject::connect(
        &server,
        &QLocalServer::newConnection,
        this,
        &LocalServer::handleNewConnection
    );

}
bool LocalServer::isOnline() const {
    return server.isListening();
}

//This is called once a new conncetion is established.
//It gets the socket, checks that it is not null - and if it is not it setups a signal
//The signal fires once, the socket tells us that something is ready to be read, and then we read it.
void LocalServer::handleNewConnection() {
    clientSocket = server.nextPendingConnection();
    qDebug() << "Client Connected to us!";
    //we get a segmentation fault without this check ;)
    if (clientSocket == nullptr) {
        qDebug() << "No client socket!";
        return;
    }
    QObject::connect(clientSocket,&QLocalSocket::readyRead,
    [this]() {
            qDebug() << "Message received!";
            readMessage();
        });
}

void LocalServer::sendMessage(QByteArrayView message, QLocalSocket *client_Socket) {
    client_Socket->write(QByteArray(message));
    client_Socket->flush();
}

//We now load in the all we can read. If we get messages such as ST -> OP -> \nSTART\n
//We now handle those correctly, since the first two will NOT go into the loop, but simply build the buffer.
void LocalServer::readMessage() {
    readBuffer += clientSocket->readAll();
    while (readBuffer.contains('\n')) {
        //Find the first \n
        const int newlineIndex = readBuffer.indexOf('\n');
        //Take all bytes before that \n
        QByteArray message = readBuffer.left(newlineIndex + 1);
        //Remove that message from the buffer.
        readBuffer.remove(0, newlineIndex + 1);
        handleMessage(message);
    }
}

