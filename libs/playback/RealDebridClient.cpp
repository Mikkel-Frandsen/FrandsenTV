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

static size_t writeCallback(
    char* ptr,
    size_t size,
    size_t nmemb,
    void* userdata)
{
    auto* data = static_cast<QByteArray*>(userdata);

    data->append(ptr, size * nmemb);

    return size * nmemb;
}QString RealDebridClient::getUnrestrictedLink(
    const QString &authString,
    const QString &link)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return {};

    QByteArray response;
    QString resultLink;

    curl_slist *headers = nullptr;

    headers = curl_slist_append(
        headers,
        authString.toUtf8().constData());

    headers = curl_slist_append(
        headers,
        "Content-Type: application/x-www-form-urlencoded");

    QByteArray encodedLink = QUrl::toPercentEncoding(link);

    QByteArray postData = "link=" + encodedLink;

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://api.real-debrid.com/rest/1.0/unrestrict/link");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        postData.constData());
    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDSIZE,
        postData.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode rc = curl_easy_perform(curl);

    long httpStatus = 0;
    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &httpStatus);

    qDebug() << "HTTP status:" << httpStatus;
    qDebug() << "Response:" << response;

    if (rc != CURLE_OK) {
        qDebug() << "curl error:" << curl_easy_strerror(rc);
    } else {
        QJsonParseError parseError;
        QJsonDocument json =
            QJsonDocument::fromJson(response, &parseError);

        if (parseError.error == QJsonParseError::NoError &&
            json.isObject()) {

            const QJsonObject obj = json.object();

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
        } else {
            qDebug() << "JSON error:"
                     << parseError.errorString();
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return resultLink;
}


void RealDebridClient::selectFileId(const QString &id,
                                    const QString &authString,
                                    const int fileId)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return;

    QByteArray response;

    // Headers
    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, authString.toUtf8().constData());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    // Endpoint
    const QString postUrl =
        "https://api.real-debrid.com/rest/1.0/torrents/selectFiles/" + id;

    // POST body
    const QByteArray postData = QString("files=%1").arg(fileId).toUtf8();
    //const QByteArray postData = QString("files=all").toUtf8();
    curl_easy_setopt(curl, CURLOPT_URL, postUrl.toUtf8().constData());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.constData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        qDebug() << "curl error:" << curl_easy_strerror(res);
    } else {
        qDebug() << "Response:" << response;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}


int RealDebridClient::getIdForSelection(QJsonObject info, const QStringList &fileFormat) {

    QJsonArray array = info["files"].toArray();
    int length = fileFormat.length();
    qDebug() << "Length fileformat" << length;
    for (const QJsonValueRef files : array) {
        QJsonObject filesObject = files.toObject();

        QString path = filesObject["path"].toString();
        qDebug() <<path;
        //Check if the last X (length of fileformat) is == last X in path
        QString pathEnd = path.right(length);
        qDebug() <<pathEnd;
        //TODO: Make a comparison of byte size, say we want the largest byte if there are two mp4 files
        for (const QString& format : fileFormat) {
            if (path.endsWith("." + format, Qt::CaseInsensitive)) {
                return filesObject["id"].toInt();
            }
        }
    }
    return -1;
}

QJsonObject RealDebridClient::getRealDebridInfo(const QString& id, const QString &authString) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return QJsonObject();
    }

    QByteArray response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);


    QByteArray auth_stringByte = authString.toUtf8();
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, auth_stringByte.constData());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    QString url = "https://api.real-debrid.com/rest/1.0/torrents/info/" + id;
    QByteArray urlByte = url.toUtf8();
    curl_easy_setopt(curl, CURLOPT_URL, urlByte.constData());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
        qDebug() << "error from getInfo" << res;
    } else {
        qDebug() << "response from getIfno" << response;
    }
    QJsonParseError error;

    const QJsonDocument json = QJsonDocument::fromJson(response, &error);
    qDebug() << "segfault?";
    QJsonObject jsonObject = json.object();

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

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return relevantJson;



}

QString RealDebridClient::getRealDebridId(const QString &infoHash, const QString &authString) {
    QByteArray auth_stringByte = authString.toLocal8Bit();
    const char* auth_string = auth_stringByte.data();


    //setup the curl
    CURL *curl = curl_easy_init();
    QByteArray response;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    //response array

    curl_slist* headers = nullptr;

    headers = curl_slist_append(headers,auth_string);
    QByteArray postData = "magnet=magnet:?xt=urn:btih:" + infoHash.toLocal8Bit();
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.real-debrid.com/rest/1.0/torrents/addMagnet");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.constData());

    // execute the request

    if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
        qDebug() << "NOT OKAY";
    } else {
        qDebug() << response;
    }
    QJsonParseError error;

    const QJsonDocument json =
        QJsonDocument::fromJson(response, &error);

    if (error.error != QJsonParseError::NoError ||
        !json.isObject()) {

        qDebug() << "Invalid JSON:"
                 << error.errorString();


        }
    QJsonObject data = json.object();
    qDebug() << data["id"].toString();
    // clean up headers after execution is complete
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (data["id"].toString() == "") {
        return response; // get full response with errors
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
    if (fileIdx == -1 or fileIdx == 0) {
        fileIdx = getIdForSelection(infoResponse, {"mkv", "mp4", "avi", "mov"});
        qDebug() << "IDn"<<fileIdx;

    }
    selectFileId(realDebridID, authQString, fileIdx);
    qDebug() << "Getting info2";
    QJsonObject infoResponse2 = getRealDebridInfo(realDebridID, authQString);
    while (getRealDebridInfo(realDebridID, authQString)["status"]== "queued") {

    }
    while (getRealDebridInfo(realDebridID, authQString)["status"]== "downloading") {
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
