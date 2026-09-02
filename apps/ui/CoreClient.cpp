//
// Created by LHF on 31/08/2026.
//
#include <QDebug>
#include "CoreClient.h"
CoreClient::CoreClient()
{
}

void CoreClient::getCatalog() {
    sendMessage(Protocol::Message{
        Protocol::MessageType::QueryGetCatalog,
{}
    });
}

void CoreClient::getTopContent(QString content, QString service, QString country) {
    QJsonObject core;
    core["content"] = content;
    core["country"] = country;
    core["service"] = service;
    sendMessage(Protocol::Message{
        Protocol::MessageType::QueryGetTopContent,
core
    });
}



void CoreClient::play(QString streamio_id) {
    QJsonObject core;
    core["streamio_id"] = streamio_id;
    sendMessage(Protocol::Message{
        Protocol::MessageType::CommandPlay,
        core
    });
}


void CoreClient::ping() {
    sendMessage(Protocol::Message{
        Protocol::MessageType::QueryPing,
{}
    });
}

void CoreClient::pause() {
    sendMessage(Protocol::Message{
        Protocol::MessageType::CommandPause,
{}
    });
}
void CoreClient::stop() {
    sendMessage(Protocol::Message{
        Protocol::MessageType::CommandStop,
{}
    });
}

void CoreClient::handleMessage(Protocol::Message message)
{
    qDebug() << "Received Message from Server";
    switch (message.type) {
        case Protocol::MessageType::CommandPause:
            qDebug() << "Received CommandPause";
            break;

        case Protocol::MessageType::CommandPlay:
            qDebug() << "Received CommandPlay";
            break;

        case Protocol::MessageType::CommandStop:
            qDebug() << "Received CommandStop";
            break;

        case Protocol::MessageType::ResponseGetTopContent: {
            qDebug() << "Received ResponseGetTopContent";
            QJsonObject data = message.data;
            const Catalog catalog = Protocol::decodeCatalog(data["catalog"].toObject());
            QVariantList movies;

            for (const Movie& movie : catalog.movies) {
                QVariantMap item;

                item["title"] = movie.title;
                item["year"] = movie.year;
                item["genre"] = movie.genre;
                item["poster"] = movie.poster_path;
                item["content_id"] = movie.imdbID;


                movies.append(item);
            }
            qDebug() << "countr" << data["country"].toString();
            qDebug() << "service" << data["service"].toString();
            qDebug() << "content" << data["content"].toString();
            emit topContentReceived(movies,
                data["country"].toString(),
                data["service"].toString(),
                data["content"].toString());

            break;

        }


        case Protocol::MessageType::ResponseGetCatalog: {
            qDebug() << "Received ResponseGetCatalog";
            QJsonObject data = message.data;
            const Catalog catalog = Protocol::decodeCatalog(data);
            QVariantList movies;

            for (const Movie& movie : catalog.movies) {
                QVariantMap item;

                item["title"] = movie.title;
                item["year"] = movie.year;
                item["genre"] = movie.genre;
                item["poster"] = movie.poster_path;
                item["content_id"] = movie.imdbID;


                movies.append(item);
            }

            emit catalogReceived(movies);

            break;

        }

        case Protocol::MessageType::ResponsePing: {
            qDebug() << "Received ResponsePing";
            QJsonObject data = message.data;

            QJsonObject core = data["core"].toObject();
            if (core.isEmpty()) {
                qWarning() << "Ping response missing core data";
                return;
            }
            QJsonObject database = data["database"].toObject();

            QString coreStatus = core["status"].toString();
            QString databaseStatus = database["status"].toString();

            qDebug() << "Core:" << coreStatus;
            qDebug() << "Database:" << databaseStatus;
            if (coreStatus == "online") {
                qDebug() << "Emitting statusChanged(true)";
                emit statusChanged(true);
            }
            break;
        }

        case Protocol::MessageType::Unknown:
            qDebug() << "Received Unknown";
            break;


        default:
            break;
    }
}


