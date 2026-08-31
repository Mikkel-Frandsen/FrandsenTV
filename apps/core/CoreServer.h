//
// Created by LHF on 31/08/2026.
//

#ifndef SMARTTV_CORESERVER_H
#define SMARTTV_CORESERVER_H
#include <QLocalServer>
#include <QDebug>
#include <QLocalSocket>


class CoreServer {
public:
    CoreServer();
    bool isOnline();
private:
    QLocalServer server;
    QLocalSocket *clientSocket = nullptr;
    void sendMessage(QByteArray, QLocalSocket*);
};



#endif //SMARTTV_CORESERVER_H
