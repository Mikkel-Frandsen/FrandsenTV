//
// Created by LHF on 26/09/2026.
//

#include "HttpClient.h"

#include <QJsonObject>

size_t HttpClient::writeCallback(
    char* ptr,
    size_t size,
    size_t nmemb,
    void* userdata) {
    auto* data = static_cast<QByteArray*>(userdata);

    data->append(ptr, size * nmemb);

    return size * nmemb;
}



HttpResponse HttpClient::get(
    const QString& url,
    const QStringList& headers)
{
    HttpResponse response;

    CURL* curl = curl_easy_init();

    if (!curl) {
        response.curlCode = CURLE_FAILED_INIT;
        return response;
    }

    QByteArray body;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::writeCallback);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    const QByteArray urlBytes = url.toUtf8();

    curl_easy_setopt(curl,CURLOPT_URL,urlBytes.constData());

    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    // Convert QStringList -> curl_slist
    curl_slist* curlHeaders = nullptr;

    for (const QString& header : headers) {
        const QByteArray headerBytes = header.toUtf8();

        curlHeaders = curl_slist_append(
            curlHeaders,
            headerBytes.constData()
        );
    }

    if (curlHeaders) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
    }

    // Perform request
    response.curlCode = curl_easy_perform(curl);

    // Save response body regardless of HTTP status
    response.body = body;

    // Get HTTP status code
    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response.statusCode
    );

    // Cleanup headers
    if (curlHeaders) {
        curl_slist_free_all(curlHeaders);
    }

    // Cleanup curl
    curl_easy_cleanup(curl);

    return response;
}

HttpResponse HttpClient::post(
    const QString &url,
    const QStringList &headers,
    const QStringList &data) {
    HttpResponse response;

    CURL* curl = curl_easy_init();

    if (!curl) {
        response.curlCode = CURLE_FAILED_INIT;
        return response;
    }

    QByteArray body;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    const QByteArray urlBytes = url.toUtf8();
    curl_easy_setopt(curl,CURLOPT_URL,urlBytes.constData());

    curl_slist* curlHeaders = nullptr;
    for (const QString& header : headers) {
        const QByteArray headerBytes = header.toUtf8();

        curlHeaders = curl_slist_append(
            curlHeaders,
            headerBytes.constData()
        );
    }

    if (curlHeaders) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
    }

    QByteArray postData = nullptr;
    for (const QString& d : data) {
        postData.append(d.toUtf8());

    }

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.constData());

    // Perform request
    response.curlCode = curl_easy_perform(curl);

    // Save response body regardless of HTTP status
    response.body = body;

    // Get HTTP status code
    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response.statusCode
    );

    // Cleanup headers
    if (curlHeaders) {
        curl_slist_free_all(curlHeaders);
    }

    // Cleanup curl
    curl_easy_cleanup(curl);

    return response;


}

QJsonObject HttpClient::convertHttpResponseToJson(const HttpResponse &response) {
    QJsonParseError error;
    const QJsonDocument json = QJsonDocument::fromJson(response.body, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Invalid JSON:"
                 << error.errorString();
        return {};
    }
    if (!json.isObject()) {
        qDebug() << "Expected JSON object";
        return {};
    }
    return json.object();
}
