//
// Created by LHF on 02/09/2026.
//
#pragma once
#include <QLocalSocket>
#include "protocol.h"

class LocalClient : public QObject {
public:
    explicit LocalClient();
    void connectToServer(const QString& serverName);
protected:
    void sendMessage(Protocol::Message);
    void readMessage();
    virtual void handleMessage(Protocol::Message message) = 0;

private:
    QLocalSocket socket;
    QByteArray readBuffer;

};








