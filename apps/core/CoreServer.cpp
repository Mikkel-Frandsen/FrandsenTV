//
// Created by LHF on 31/08/2026.
//

#include "CoreServer.h"
CoreServer::CoreServer(const QString &serverName)
    : LocalServer(serverName),
      playbackManager(this),
      databaseManager(),
      tmdbManager(),
      catalogRep(&databaseManager, &tmdbManager), // Passes living references
      contentManager(&catalogRep)            // Passes CatalogRep pointer
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

            QString imdbId = contentManager.getImdbId(streamioID);

            qDebug() << "Received CommandPlay :" <<streamioID;
            qDebug() << "Received CommandPlay :" <<imdbId;

            qDebug() << "PlaybackManager:" << &playbackManager;
            qDebug() << "BEFORE playMovie";
            playbackManager.playMovie(imdbId);
            qDebug() << "AFTER playMovie";

            break;
        }
        case Protocol::MessageType::CommandStop: {
            qDebug() << "Received CommandStop";
            break;
        }

        case Protocol::MessageType::QueryGetMovie: {
        }
        case Protocol::MessageType::QueryGetPage: {
            qDebug() << "Received ResponseGetPage";
            QJsonObject data = message.data;

            const int page = data["page"].toInt();
            QVector<Catalog> pageCatalogs = contentManager.getPage(static_cast<Protocol::Page>(page));
            QJsonArray catalogs;
            for (const Catalog& catalog : pageCatalogs) {
                catalogs.append(Protocol::encodeCatalog(catalog));
            }

            QJsonObject responseData;
            responseData["catalogs"] = catalogs;
            responseData["page"] = page;
            Protocol::Message response{
                Protocol::MessageType::ResponseGetPage,
                responseData
            };
            qDebug() << "Sending ResponseGetPage";
            sendMessage(
                    response,
                    clientSocket
                );
            break;

        }
        case Protocol::MessageType::QueryGetTopContent: {

            qDebug() << "Received QueryGetTopContent";

            const QString content =
                message.data["content"].toString();

            const QString service =
                message.data["service"].toString();

            const QString country =
                message.data["country"].toString();


            Catalog catalog =
                databaseManager.getTopMovies(service, country);

            catalog.id = "topContent";
            catalog.title = service ;

            qDebug() << "Creating Response";

            QJsonObject responseData;
            responseData["catalog"] =
                Protocol::encodeCatalog(catalog);

            responseData["content"] = content;
            responseData["service"] = service;
            responseData["country"] = country;

            Protocol::Message response{
                Protocol::MessageType::ResponseGetTopContent,
                responseData
            };



            qDebug() << "Sending ResponseGetTopContent";

            sendMessage(response, clientSocket);

            break;
        }

        case Protocol::MessageType::QueryGetCatalog: {
            qDebug() << "Received QueryGetCatalog";
            Catalog catalog = databaseManager.getCatalog();
            catalog.title = "All Movies";
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
