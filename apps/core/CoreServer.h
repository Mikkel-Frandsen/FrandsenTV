//
// Created by LHF on 31/08/2026.
//

#pragma once

#include "localServer.h"
#include "DatabaseManager.h"
#include "PlaybackManager.h"
class CoreServer : public LocalServer {
    Q_OBJECT
public:
    CoreServer(const QString& serverName);

    DatabaseManager databaseManager; // TODO make private, just here to test
    PlaybackManager playbackManager; // TODO make private, just here to test

protected:
    void handleMessage(Protocol::Message, QLocalSocket*) override;

private:
    void handleQuery(QByteArrayView, QLocalSocket*);

    void handleCommand(QByteArrayView, QLocalSocket*);
};


