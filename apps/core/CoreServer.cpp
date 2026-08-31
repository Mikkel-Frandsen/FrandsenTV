//
// Created by LHF on 31/08/2026.
//

#include "CoreServer.h"


CoreServer::CoreServer()
{
    if (!server.listen("smartTV-core")) {
        qWarning() << server.errorString();
    }
    //This creates a signal for the server, that fires once a newConnection is achived.
    //It stores the connection in ClientSocket, for furhter uses

    QObject::connect(
    &server,
    &QLocalServer::newConnection,
    [this]() {
        clientSocket = server.nextPendingConnection();
        qDebug() << "Client Connected to us!";
        qDebug() << clientSocket;
        //we get a segmentation fault without this check ;)
        if (clientSocket == nullptr) {
            qDebug() << "No client socket!";
            return;
        }
        QObject::connect(
            clientSocket,
            &QLocalSocket::readyRead,
        [this]() {
                qDebug() << "Message received!";
                QByteArray message = clientSocket->readAll();
                if (message == "STATUS") {
                    qDebug() << "Received STATUS request!";
                    sendMessage("ONLINE", clientSocket);
                }
            });
        });




}
bool CoreServer::isOnline() {
    return server.isListening();
}

void CoreServer::sendMessage(QByteArray message, QLocalSocket *client_Socket) {
    client_Socket->write(message);
    client_Socket->flush();

}
