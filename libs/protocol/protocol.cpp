

#include "protocol.h"

namespace Protocol {
    QByteArray encode(const Message& message) {
        QByteArray result;

        // https://stackoverflow.com/questions/36761501/convert-an-int-to-qbytearray-in-qt
        const int type = static_cast<int>(message.type);

        result.append(
            reinterpret_cast<const char*>(&type),
            sizeof(type)
        );
        // https://forum.qt.io/topic/134074/how-to-convert-qjsonobject-to-qbytearray/2
        QJsonDocument json(message.data);

        result.append(
            json.toJson(QJsonDocument::Compact)
        );

        result.append("\n");
        return result;

    }

    Message decode(const QByteArray& message)
    {
        if (message.isEmpty()) {
            qDebug() << "Empty Message";
            return {MessageType::Unknown, {}};
        }

        if (message.size() < static_cast<qsizetype>(sizeof(int))) {
            qDebug() << "Not even message type received";
            return {MessageType::Unknown, {}};
        }

        int type = 0;

        std::memcpy(
            &type,
            message.constData(),
            sizeof(type)
        );

        qDebug() << "Message type:" << type;

        QByteArray jsonBytes =
            message.sliced(sizeof(int));

        QJsonParseError error;

        QJsonDocument json =
            QJsonDocument::fromJson(jsonBytes, &error);

        if (error.error != QJsonParseError::NoError ||
            !json.isObject()) {

            qDebug() << "Invalid JSON:"
                     << error.errorString();

            return {MessageType::Unknown, {}};
            }

        QJsonObject data = json.object();

        return {
            static_cast<MessageType>(type),
            data
        };
    }

    QJsonObject encodeCatalog(const Catalog& catalog) {
        QJsonObject json;
        QJsonArray movies;

        for (const Movie& movie : catalog.movies) {
            QJsonObject movieJson;
            movieJson["desc"] = movie.description;
            movieJson["title"] = movie.title;
            movieJson["poster"] = movie.poster_path;
            movieJson["year"] = movie.year;
            movieJson["genre"] = movie.genre;
            movieJson["stremioID"] = movie.stremioID;
            movieJson["imdbID"] = movie.imdbID;



            movies.append(movieJson);
        }

        json["movies"] = movies;
        json["title"] = catalog.title;
        json["id"] = catalog.id;

        return json;


    }

    Catalog decodeCatalog(const QJsonObject& json) {
        Catalog catalog;
        QJsonArray movies = json["movies"].toArray();

        for (const QJsonValue& value : movies) {
            const QJsonObject movieJson =
                value.toObject();

            Movie movie;

            movie.description   = movieJson["desc"].toString();
            movie.title         = movieJson["title"].toString();
            movie.poster_path   = movieJson["poster"].toString();
            movie.year          = movieJson["year"].toInt();
            movie.genre         = movieJson["genre"].toString();
            movie.stremioID     = movieJson["stremioID"].toString();
            movie.imdbID        = movieJson["imdbID"].toString();

            catalog.movies.append(movie);
        }
        catalog.title = json["title"].toString();
        catalog.id = json["id"].toString();

        return catalog;
    }
}