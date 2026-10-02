//
// Created by LHF on 31/08/2026.
//

#pragma once

#include "ContentManager.h"
#include "localServer.h"
#include "DatabaseManager.h"
#include "PlaybackManager.h"
#include "CatalogRep.h"

class CoreServer : public LocalServer {
    Q_OBJECT
public:
    CoreServer(const QString& serverName);


protected:
    void handleMessage(Protocol::Message, QLocalSocket*) override;


private:
    void handleQuery(QByteArrayView, QLocalSocket*);

    void handleCommand(QByteArrayView, QLocalSocket*);
    PlaybackManager playbackManager;

    DatabaseManager databaseManager;
    TMDBManager tmdbManager;
    CatalogRep catalogRep;

    ContentManager contentManager;
};


