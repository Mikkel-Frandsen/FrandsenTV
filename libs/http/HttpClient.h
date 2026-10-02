#pragma once
#include <QJsonObject>

#include "HttpResponse.h"


class HttpClient {
public:
    static HttpResponse get(
        const QString& url,
        const QStringList& headers);
    HttpResponse post(
        const QString& url,
        const QStringList& headers,
        const QStringList& data);
    QJsonObject convertHttpResponseToJson(const HttpResponse& response);
private:
    static size_t writeCallback(char* ptr,
                                size_t size,
                                size_t nmemb,
                                void* userdata);
};


