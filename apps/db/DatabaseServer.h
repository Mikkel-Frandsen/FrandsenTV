//
// Created by LHF on 02/09/2026.
//
# pragma once

#include <QLocalServer>
#include <QDebug>
#include <QLocalSocket>

class DatabaseServer : public QObject {
    Q_OBJECT
public:
    DatabaseServer();
    bool isOnline() const;
private:
    QLocalServer server;
    QLocalSocket *clientSocket = nullptr;

    QByteArray readBuffer;
};


