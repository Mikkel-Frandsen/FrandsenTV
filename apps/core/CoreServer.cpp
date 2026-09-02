//
// Created by LHF on 31/08/2026.
//

#include "CoreServer.h"

CoreServer::CoreServer(const QString& serverName)
    :   LocalServer(serverName),
        playbackManager(this)
{

}

void CoreServer::handleMessage(Protocol::Message message, QLocalSocket *clientSocket) {
    switch (message.type) {
        case Protocol::MessageType::CommandPause:
            qDebug() << "Received CommandPause";
            break;

        case Protocol::MessageType::CommandPlay: {
            //todo make a check that this is actually the correctdata type (QSTING)
            QJsonObject data = message.data;
            QString streamioID = data["streamio_id"].toString();
            qDebug() << "Received CommandPlay :" <<streamioID;
            qDebug() << "PlaybackManager:" << &playbackManager;
            qDebug() << "BEFORE playMovie";
            playbackManager.playMovie(streamioID);
            qDebug() << "AFTER playMovie";
            break;
        }
        case Protocol::MessageType::CommandStop: {
            qDebug() << "Received CommandStop";
            break;
        }

        case Protocol::MessageType::QueryGetMovie: {
        }
        case Protocol::MessageType::QueryGetTopContent: {

            qDebug() << "Received QueryGetTopContent";
            //return a catalog of topMovies (needs service, country)
            //message.data["country, content, service"]...


            Catalog catalog = databaseManager.getTopMovies(
                message.data["service"].toString(),
                message.data["country"].toString());

            qDebug() << "Creating Response";
            QJsonObject res = message.data;
            res["catalog"] = Protocol::encodeCatalog(catalog);
            Protocol::Message response{
                Protocol::MessageType::ResponseGetTopContent,
                res
            };
            qDebug() << "Sending ResponseGetTopContent";
            sendMessage(
                    response,
                    clientSocket
                );
            break;
        }


        case Protocol::MessageType::QueryGetCatalog: {

            qDebug() << "Received QueryGetCatalog";
            Catalog catalog = databaseManager.getCatalog();
            qDebug() << "Creating Response";

            Protocol::Message response{
                Protocol::MessageType::ResponseGetCatalog,
                Protocol::encodeCatalog(catalog)
            };
            qDebug() << "Sending ResponseGetCatalog";
            sendMessage(
                    response,
                    clientSocket
                );
            break;
        }
        case Protocol::MessageType::QueryPing: {
            qDebug() << "Received QueryPing";

            QJsonObject core;
            core["status"] = "online";

            QJsonObject database;
            database["status"] = "connected";

            QJsonObject data;
            data["core"] = core;
            data["database"] = database;

            Protocol::Message response_message = {
                Protocol::MessageType::ResponsePing,
                data
            };

            sendMessage(
                response_message,
                clientSocket
            );

            break;
        }
            
        case Protocol::MessageType::Unknown:
            qDebug() << "Received Unknown";
            break;


        default:
            break;
    }
}
