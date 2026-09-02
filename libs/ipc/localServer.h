//
// Created by LHF on 02/09/2026.
//

#pragma once
#include <QLocalServer>
#include <QDebug>
#include <QLocalSocket>
#include "protocol.h"

class LocalServer : public QObject {
    Q_OBJECT
public:
    explicit LocalServer(const QString& serverName);
    bool isOnline() const;

protected:

    //IPS SETUP
    void handleNewConnection();

    //SEND & RECEIVE
    void sendMessage(Protocol::Message, QLocalSocket*);
    void readMessage();
    virtual void handleMessage(Protocol::Message, QLocalSocket*) = 0; //Requires each subclass to implement this
    //Created this so subclasses can have their own handleQuery
    //(This replaces the inner part of old read message)

private:
    QLocalServer server;
    QLocalSocket *clientSocket = nullptr;
    QByteArray readBuffer;
};






