//
// Created by LHF on 15/09/2026.
//

#include "RealDebridClient.h"
#include <curl/curl.h>
#include <QProcessEnvironment>
#include <QFile>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonObject>

RealDebridClient::RealDebridClient(QObject* parent)
    : QObject(parent), networkManager(nullptr) {
}

QString RealDebridClient::buildMagnetLink(const QString& infoHash)
{
    return "magnet:?xt=urn:btih:" + infoHash;
}

QString RealDebridClient::getUnrestrictedLink(
    const QString &authString,
    const QString &link)
{
    //URL
    QString url = "https://api.real-debrid.com/rest/1.0/unrestrict/link";
    //HEADERS
    QStringList headers;
    headers.append(authString);
    headers.append("Content-Type: application/x-www-form-urlencoded");
    //POST DATA
    QStringList postData;

    QByteArray encodedLink = QUrl::toPercentEncoding(link);
    QString postData1 = "link=" + encodedLink;
    postData.append(postData1);
    //GET RESPONSE
    HttpResponse response = httpClient.post(url, headers, postData);
    if (!response.httpSucess()) {
        qDebug() << "Real-Debrid HTTP request failed:"  << response.statusCode;
        return {};
    }
    if (!response.networkSucces()) {
        qDebug() << "Real-Debrid CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    QJsonObject obj = httpClient.convertHttpResponseToJson(response);
    QString resultLink;


    if (obj.contains("download")) {
        resultLink =
                obj.value("download").toString();
    } else if (obj.contains("error")) {
        qDebug()
                << "Real-Debrid error:"
                << obj.value("error").toString()
                << "code:"
                << obj.value("error_code").toInt();
    }


    return resultLink;
}


void RealDebridClient::selectFileId(const QString &id,
                                    const QString &authString,
                                    const int fileId)
{
    //URL
    const QString url =
        "https://api.real-debrid.com/rest/1.0/torrents/selectFiles/" + id;
    //HEADERS
    QStringList headers;
    headers.append(authString);
    headers.append("Content-Type: application/x-www-form-urlencoded");
    //POST DATA
    QStringList postData;
    QString postData1 = QString("files=%1").arg(fileId);
    postData.append(postData1);
    //We dont save the response here ;), we just want to "click" the right file
    httpClient.post(url, headers, postData);
}


int RealDebridClient::getIdForSelection(QJsonObject info, const QStringList &fileFormat) {
    QJsonArray array = info["files"].toArray();
    int length = 3;
    for (const QJsonValueRef files : array) {
        QJsonObject filesObject = files.toObject();

        QString path = filesObject["path"].toString();

        //Check if the last X (length of fileformat) is == last X in path
        QString pathEnd = path.right(length);
        //TODO: Make a comparison of byte size, say we want the largest byte if there are two mp4 files
        for (const QString& format : fileFormat) {
            qDebug() << pathEnd;
            if (path.endsWith("." + format, Qt::CaseInsensitive)) {
                return filesObject["id"].toInt();
            }
        }
    }
    return -1;
}

QJsonObject RealDebridClient::getRealDebridInfo(const QString& id, const QString &authString) {
    //URL
    QString url = "https://api.real-debrid.com/rest/1.0/torrents/info/" + id;
    //HEADERS
    QStringList headers;
    headers.append(authString);
    //GET RESPONSE
    HttpResponse response = httpClient.get(url, headers);

    if (!response.networkSucces()) {
        qDebug() << "Real-Debrid CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Real-Debrid HTTP request failed:"  << response.statusCode;
        qDebug() << response.body;
        return {};
    }


    QJsonObject jsonObject = httpClient.convertHttpResponseToJson(response);

    QJsonObject relevantJson;
    QStringList relevantFields = {"status", "files"};
    for (const QString &fields : relevantFields) {
        if (jsonObject.contains(fields)) {
            relevantJson.insert(fields, jsonObject.value(fields));
        }
    }
    if (relevantJson["status"] == "downloaded") {
        relevantJson.insert("links", (jsonObject["links"]));
    }
    else if (relevantJson["status"] == "downloading") {
        relevantJson.insert("seeders", (jsonObject["seeders"]));
    }



    return relevantJson;



}

QString RealDebridClient::getRealDebridId(const QString &infoHash, const QString &authString) {
    //URL
    QString url = "https://api.real-debrid.com/rest/1.0/torrents/addMagnet";
    //HEADERS
    QStringList headers;
    headers.append(authString);
    //POST DATA
    QStringList postData;
    QString magnetLink = "magnet=magnet:?xt=urn:btih:" + infoHash;
    postData.append(magnetLink);
    //GET RESPONSE
    HttpResponse response = httpClient.post(url, headers, postData);

    if (!response.networkSucces()) {
        qDebug() << "Real-Debrid CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Real-Debrid HTTP request failed:"  << response.statusCode;
        qDebug() << "Response:" << response.body;
        return {};
    }


    QJsonObject data = httpClient.convertHttpResponseToJson(response);
    qDebug() << data["id"].toString();
    // clean up headers after execution is complete

    if (data["id"].toString() == "") {
        return response.body; // get full response with errors
    }
    return data["id"].toString();
}

void RealDebridClient::getStreamLink(const QString& infoHash, int fileIdx) {


    QString realDebridToken =
    QProcessEnvironment::systemEnvironment()
        .value("REALDEBRID_API_KEY");

    QString authQString = "Authorization: Bearer " + realDebridToken;
    qDebug() << "Getting ID" << infoHash;
    QString realDebridID = getRealDebridId(infoHash, authQString);
    qDebug() << "returned id : " << realDebridID;
    if (realDebridID.contains("\"error_code\"") and realDebridID.contains("\"error\"")) {
        qDebug() << "Found error in ID";
        emit streamError("");
        return;
    }
    qDebug() << "Getting info";
    QJsonObject infoResponse = getRealDebridInfo(realDebridID, authQString);
    qDebug() << "info 1 response" << infoResponse;
    qDebug() << fileIdx;
    int fileIdx1 = getIdForSelection(infoResponse, {"mkv", "mp4", "avi", "mov"});
    qDebug() << fileIdx1;

    selectFileId(realDebridID, authQString, fileIdx1);
    qDebug() << "Getting info2";
    QJsonObject infoResponse2 = getRealDebridInfo(realDebridID, authQString);
    while (getRealDebridInfo(realDebridID, authQString)["status"]== "queued") {

    }
    while (getRealDebridInfo(realDebridID, authQString)["status"]== "downloading") {
        qDebug() << "Downloading";
        if (getRealDebridInfo(realDebridID, authQString)["seeders"]== 0) {
            emit streamError("");
            return;
        }
    }
    qDebug() << infoResponse2["links"];
    QJsonArray linkArray =  infoResponse2["links"].toArray();
    QString link = linkArray[0].toString();
    //WAIT UNTIL IT IS DOWNLOADED!!!
    //WORKFLOW OF REAL DEBRID COMMUNCATION
        // short:
    qDebug() << link;
    QString unrestrictedLink = getUnrestrictedLink(authQString, link);
    if (unrestrictedLink == "") {
        emit streamError("");
        return;
    }
    qDebug() << unrestrictedLink;
    //BIG BAD BUNNY
    //dd8255ecdc7ca55fb0bbf81323d87062db1f6d1c

    emit streamReady(unrestrictedLink);
    //curl -X POST
    //-H "Authorization: Bearer API KEY"
    //--data-urlencode "magnet=magnet:?xt=urn:btih:dd8255ecdc7ca55fb0bbf81323d87062db1f6d1c"
    //"https://api.real-debrid.com/rest/1.0/torrents/addMagnet"


    //ANSWER:
    //{
    //"id": "BRPHEJKS6MXOG",
    //"uri": "https:\/\/api.real-debrid.com\/rest\/1.0\/torrents\/info\/BRPHEJKS6MXOG"
    //}



    //NEXT:
    //curl -X GET -H "Authorization: Bearer API KEY"
    //"https://api.real-debrid.com/rest/1.0/torrents/info/{ID}"

    //ANSWER

    /*
    {
        "id": "BRPHEJKS6MXOG",
        "filename": "Big Buck Bunny",
        "original_filename": "Big Buck Bunny",
        "hash": "dd8255ecdc7ca55fb0bbf81323d87062db1f6d1c",
        "bytes": 276445467,
        "original_bytes": 276445467,
        "host": "real-debrid.com",
        "split": 2000,
        "progress": 0,
        "status": "waiting_files_selection",
        "added": "2026-09-23T17:42:13.000Z",
        "files": [
                {
                        "id": 1,
                        "path": "\/Big Buck Bunny.en.srt",
                        "bytes": 140,
                        "selected": 0
                },
                {
                        "id": 2,
                        "path": "\/Big Buck Bunny.mp4",
                        "bytes": 276134947,
                        "selected": 0
                },
                {
                        "id": 3,
                        "path": "\/poster.jpg",
                        "bytes": 310380,
                        "selected": 0
                }
        ],
        "links": [

        ]
}

     */

        //Then we POST same auth bearer, to
        //-d "files=2" "https://api.real-debrid.com/rest/1.0/torrents/selectFiles/FV2FA5L2HDOWO"
        //Since the ID file (the .mp4 had id 2)

        //This does not answer
        //But we can check the status by doing a GET to
        //"https://api.real-debrid.com/rest/1.0/torrents/info/FV2FA5L2HDOWO"

        //This provides a status and a links.
        //Once status==downloaded
        //we can take the link in links, and
        //send a



}
