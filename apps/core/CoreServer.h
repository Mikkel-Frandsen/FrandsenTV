//
// Created by LHF on 31/08/2026.
//

#pragma once

#include "localServer.h"
class CoreServer : public LocalServer {
    Q_OBJECT
public:
    CoreServer(const QString& serverName);
protected:
    void handleMessage(QByteArray, QLocalSocket*) override;
private:
    void handleQuery(QByteArrayView, QLocalSocket*);
    void handleCommand(QByteArrayView, QLocalSocket*);
};


