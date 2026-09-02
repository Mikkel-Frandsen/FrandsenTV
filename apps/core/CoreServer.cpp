//
// Created by LHF on 31/08/2026.
//

#include "CoreServer.h"
#include "../../libs/protocol/coreProtocol.h"

CoreServer::CoreServer()
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
        &CoreServer::handleNewConnection
    );

}
bool CoreServer::isOnline() const {
    return server.isListening();
}

//This is called once a new conncetion is established.
//It gets the socket, checks that it is not null - and if it is not it setups a signal
//The signal fires once, the socket tells us that something is ready to be read, and then we read it.
void CoreServer::handleNewConnection() {
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

void CoreServer::sendMessage(QByteArrayView message, QLocalSocket *client_Socket) {
    client_Socket->write(QByteArray(message));
    client_Socket->flush();
}

//We now load in the all we can read. If we get messages such as ST -> OP -> \nSTART\n
//We now handle those correctly, since the first two will NOT go into the loop, but simply build the buffer.
void CoreServer::readMessage() {
    readBuffer += clientSocket->readAll();
    while (readBuffer.contains('\n')) {
        //Find the first \n
        const int newlineIndex = readBuffer.indexOf('\n');
        //Take all bytes before that \n
        QByteArray message = readBuffer.left(newlineIndex + 1);
        //Remove that message from the buffer.
        readBuffer.remove(0, newlineIndex + 1);
        switch (CoreProtocol::getMessageType(message)) {
            case Protocol::MessageType::Query:
                qDebug() << "A query message";
                handleQuery(message);
                break;
            case Protocol::MessageType::Command:
                qDebug() << "A command message";

                handleCommand(message);
                break;
            default:
                break;
        }
    }
}

//This handles any queries (currently with a if statement)
void CoreServer::handleQuery(QByteArrayView query) {
    if (query == CoreProtocol::Query::GetStatus) {
        qDebug() << "Received STATUS query!";
        sendMessage(CoreProtocol::Event::Online, clientSocket);
    }
}

//This handles any commands (currently with a if statement)
//Currently they dont actually cause a state change
// ACK = command was received and accepted.
// Event = resulting state change actually occurred.
void CoreServer::handleCommand(QByteArrayView command) {
    if (command == CoreProtocol::Command::Pause) {
        qDebug() << "Received PAUSE command!";
        sendMessage(CoreProtocol::Acknowledgment::AckPause, clientSocket);

        //      ------
        //...Actually pause...
        //      ------

        sendMessage(CoreProtocol::Event::PlaybackPaused, clientSocket);
    }
    if (command == CoreProtocol::Command::Stop) {
        qDebug() << "Received STOP command!";
        sendMessage(CoreProtocol::Acknowledgment::AckStop, clientSocket);
        //      ------
        //...Actually stop...
        //      ------

        sendMessage(CoreProtocol::Event::PlaybackStopped, clientSocket);
    }
}