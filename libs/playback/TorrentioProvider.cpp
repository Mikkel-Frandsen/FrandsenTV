#include "TorrentioProvider.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonArray>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

TorrentioProvider::TorrentioProvider(QObject* parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void TorrentioProvider::getStreams(const QString& stremioId)
{
    qDebug() << "inside getStreams";
    QUrl url(
        "https://torrentio.strem.fun/"
        "qualityfilter=720p,480p,cam,scr|"
        "/stream/movie/"
        + stremioId
        + ".json"
    );

    QNetworkRequest request(url);

    QNetworkReply* reply = networkManager->get(request);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            if (reply->error() != QNetworkReply::NoError) {
                emit streamsError(reply->errorString());
                reply->deleteLater();
                return;
            }

            const QByteArray responseData = reply->readAll();

            QJsonParseError parseError;
            const QJsonDocument jsonDoc =
                QJsonDocument::fromJson(responseData, &parseError);

            if (parseError.error != QJsonParseError::NoError) {
                emit streamsError(parseError.errorString());
                reply->deleteLater();
                return;
            }

            if (!jsonDoc.isObject()) {
                emit streamsError("Torrentio response is not a JSON object");
                reply->deleteLater();
                return;
            }

            const QJsonObject jsonObject = jsonDoc.object();
            qDebug() << jsonObject;
            const QList<Stream> streams = jsonToStreams(jsonObject);
            emit streamsReady(streams);
            reply->deleteLater();
        }
    );
}




QList<Stream> TorrentioProvider::jsonToStreams(const QJsonObject& json)
{
    QList<Stream> streams;
    QJsonArray streamsJson = json["streams"].toArray();
    for(QJsonValueRef streamJson : streamsJson) {
        QJsonObject streamJsonObj = streamJson.toObject();
        Stream stream;
        stream.name = streamJsonObj["name"].toString();
        stream.bingeGroup = (streamJsonObj["behaviorHints"].toObject())["bingeGroup"].toString();
        qDebug() << streamJsonObj;
        //qDebug() << stream.bingeGroup;
        qDebug() << "fileidx" << streamJsonObj["fileIdx"];
        if (streamJsonObj["fileIdx"].isNull()) {
            stream.fileIdx = -1;
        } else {
            stream.fileIdx = streamJsonObj["fileIdx"].toInt();
        }
        stream.infoHash = streamJsonObj["infoHash"].toString();
        stream.title = streamJsonObj["title"].toString();
        //todo: make the filtering here
        QString filename = streamJsonObj["filename"].toString()
                                + stream.title
                                + stream.bingeGroup;

        streams.append(stream);

    }
    return streams;
}