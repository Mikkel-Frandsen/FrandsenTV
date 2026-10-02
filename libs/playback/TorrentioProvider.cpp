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
    //https://torrentio.strem.fun/providers=yts,eztv,rarbg,1337x,ext,thepiratebay,kickasstorrents,torrentgalaxy,magnetdl,horriblesubs,nyaasi,tokyotosho,anidex,nekobt|qualityfilter=scr,unknown,cam,480p,720p/manifest.json
    qDebug() << "inside getStreams";
    QUrl url(
        "https://torrentio.strem.fun/"
        "providers=yts,eztv,rarbg,1337x,ext,thepiratebay,kickasstorrents,torrentgalaxy,magnetdl,horriblesubs,nyaasi,tokyotosho,anidex,nekobt|"
        "qualityfilter=480p,cam,scr,unknown|"
        "sort=seeders"
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
            const QList<Stream> streams = jsonToStreams(jsonObject);
            emit streamsReady(streams);
            reply->deleteLater();
        }
    );
}

//https://www.patreon.com/debridmediamanager/posts/complete-list-of-158388927
bool containsFilters(const QString& filename)
{
    // Rule 1: substring matches
    static const QStringList substringFilters = {
        "web-dl",
        "webrip",
        "bdrip",
        "hdrip",
        "dvdrip"
    };

    for (const QString& filter : substringFilters) {
        if (filename.contains(filter, Qt::CaseInsensitive)) {
            return true;
        }
    }

    // Rule 2: source.codec combinations
    static const QStringList dotFilters = {
        "bluray.x264",
        "hdtv.x264",
        "hdtv.xvid",
        "web.x264",
        "web.h264"
    };

    for (const QString& filter : dotFilters) {
        if (filename.contains(filter, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
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
        if (!containsFilters(filename)) {
            streams.append(stream);
        }
    }
    return streams;
}