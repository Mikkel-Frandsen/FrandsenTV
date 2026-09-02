//
// Created by LHF on 31/08/2026.
//

#pragma once

#include <QLocalServer>
#include <QDebug>
#include <QLocalSocket>

class CoreServer : public QObject {
    Q_OBJECT
public:
    CoreServer();
    bool isOnline() const;
private:
    QLocalServer server;
    QLocalSocket *clientSocket = nullptr;

    QByteArray readBuffer;

    static void sendMessage(QByteArrayView, QLocalSocket*);
    void readMessage();
    void handleQuery(QByteArrayView);
    void handleCommand(QByteArrayView);
    void handleNewConnection();

};


