//
// Created by LHF on 31/08/2026.
//
#include <QDebug>
#include "../../libs/protocol/coreProtocol.h"


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
        this,
        &CoreClient::readMessage
        );



}

void CoreClient::connectToCore() {
    socket.connectToServer("smartTV-core");
}



// ----- Sending of messages (Queries/Commands) -----

//Centrilized function to send messages. Ensures we are connected to the server
void CoreClient::sendMessage(QByteArrayView message) {
    if (socket.state() != QLocalSocket::ConnectedState) {
        qWarning() << "Cannot send message: not connected";
        return;
    }
    socket.write(QByteArray(message));
    socket.flush();
}


void CoreClient::getStatus() {
    sendMessage(CoreProtocol::Query::GetStatus);
}

void CoreClient::pause() {
    sendMessage(CoreProtocol::Command::Pause);
}

void CoreClient::stop() {
    sendMessage(CoreProtocol::Command::Stop);
}

// ----------------------------------------




void CoreClient::readMessage() {
    readBuffer += socket.readAll();
    while (readBuffer.contains('\n')) {
        //Find the first \n
        const int newlineIndex = readBuffer.indexOf('\n');
        //Take all bytes before that \n
        QByteArray message = readBuffer.left(newlineIndex + 1);
        //Remove that message from the buffer.
        readBuffer.remove(0, newlineIndex + 1);

        switch (CoreProtocol::getMessageType(message)) {
            case Protocol::MessageType::Event:
                handleEvent(message);
                break;
            case Protocol::MessageType::Acknowledgment:
                handleAcknowledgment(message);
                break;
            default:
                break;
        }
    }
}


void CoreClient::handleAcknowledgment(QByteArrayView message) {
    if (message == CoreProtocol::Acknowledgment::AckPause) {
        qDebug() << "Received ACK for Pause";
        //Do something here. If we dont get an ack, what should the protocol do
    } else if (message == CoreProtocol::Acknowledgment::AckStop) {
        qDebug() << "Received ACK for Stop";
        //Do something here. If we dont get an ack, what should the protocol do
    }
}

void CoreClient::handleEvent(QByteArrayView message) {
    if (message == CoreProtocol::Event::Online) {
        qDebug() << "Recieved ONLINE status";
        emit statusChanged(true);
    } else if (message == CoreProtocol::Event::PlaybackPaused) {
        qDebug() << "Recieved PLAYBACK_PAUSED";
        emit playbackPaused();
    } else if (message == CoreProtocol::Event::PlaybackStopped) {
        qDebug() << "Recieved PLAYBACK_STOPPED";
        emit playbackStopped();
    }
}


