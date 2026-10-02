#pragma once
#include <QByteArray>
#include <curl/curl.h>

struct HttpResponse {
    int statusCode = 0;
    QByteArray body;
    CURLcode curlCode;

    bool networkSucces() {
        return curlCode == CURLE_OK;
    }

    bool httpSucess() {
        return statusCode >= 200 and statusCode < 300;
    }


};
