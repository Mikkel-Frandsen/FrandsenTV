//
// Created by LHF on 31/08/2026.
//

#include "CoreServer.h"
#include "../../libs/protocol/coreProtocol.h"

CoreServer::CoreServer(const QString& serverName)
    : LocalServer(serverName)
{
}

void CoreServer::handleMessage(QByteArray message, QLocalSocket *clientSocket) {
    switch (CoreProtocol::getMessageType(message)) {
        case Protocol::MessageType::Query:
            qDebug() << "A query message";
            handleQuery(message, clientSocket);
            break;
        case Protocol::MessageType::Command:
            qDebug() << "A command message";

            handleCommand(message, clientSocket);
            break;
        default:
            break;
    }
}


//This handles any queries (currently with a if statement)
void CoreServer::handleQuery(QByteArrayView query, QLocalSocket *clientSocket) {
    if (query == CoreProtocol::Query::GetStatus) {
        qDebug() << "Received STATUS query!";
        sendMessage(CoreProtocol::Event::Online, clientSocket);
    }
}

//This handles any commands (currently with a if statement)
//Currently they dont actually cause a state change
// ACK = command was received and accepted.
// Event = resulting state change actually occurred.
void CoreServer::handleCommand(QByteArrayView command, QLocalSocket *clientSocket) {
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