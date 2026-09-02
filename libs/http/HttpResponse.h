#pragma once
#include <QByteArray>
#include <curl/curl.h>

struct HttpResponse {
    int statusCode;
    QByteArray body;



};
