//
// Created by LHF on 13/09/2026.
//

#include "SyncManager.h"

#include <curl/curl.h>
#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcessEnvironment>

#include "movie.h"

SyncManager::SyncManager() {
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
}


Movie parseTMBDJsonMovie(QJsonObject json_movie) {
    qDebug() << "parsemovie" << json_movie;

    Movie movie;
    int id = json_movie["id"].toInt();
    QString id_str = QString::number(id);
    movie.stremioID =  id_str;
    movie.title =  json_movie["original_title"].toString();
    movie.description = json_movie["overview"].toString();
    movie.imdbID =json_movie["imdb_id"].toString();
    movie.poster_path = json_movie["poster_path"].toString();

    return movie;

}

Movie SyncManager::getMovieTMDB(const QString &tmbd_id) {
    //setup the curl
    CURL *hnd = curl_easy_init();
    QByteArray response;
    QString url =
    "https://api.themoviedb.org/3/movie/" + tmbd_id;

    QByteArray urlUtf8 = url.toUtf8();

    curl_easy_setopt(
        hnd,
        CURLOPT_URL,
        urlUtf8.constData()
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEFUNCTION,
        writeCallback
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEDATA,
        &response
    );

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "accept: application/json"
    );

    headers = curl_slist_append(
        headers,
        "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJiMzQyOTNlNDA0NGQwM2I4MzQyOWJiNTE4Y2IzM2NlYiIsIm5iZiI6MTc4OTMzMDc5NS45MTY5OTk4LCJzdWIiOiI2YWE3MDU2YmZkODVjMjM0NDVjMTdjNDIiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0.HQgfX1GYfEB8Oyxg112EmUMUBKdsjKrgw3RxAWEEa_w"
    );


    curl_easy_setopt(
        hnd,
        CURLOPT_HTTPHEADER,
        headers
    );


    CURLcode ret = curl_easy_perform(hnd);

    if (ret != CURLE_OK) {
        qDebug() << "CURL error:"
                 << curl_easy_strerror(ret);

        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);

        return {};
    }

    QJsonParseError error;

    QJsonDocument json = QJsonDocument::fromJson(response, &error);

    if (error.error != QJsonParseError::NoError ||
    !json.isObject())
    {
        qDebug() << "Invalid JSON:"
                 << error.errorString();

        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);

        return {};
    }

    QJsonObject data = json.object();

    Movie movie = parseTMBDJsonMovie(data);
    qDebug() << movie.imdbID;
    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);
    qDebug() << "Finished";
    return movie;

}


QString SyncManager::getImdbId(int tmdbId)
{
    CURL* hnd = curl_easy_init();

    if (!hnd)
        return {};

    QByteArray response;

    QString url = QString(
        "https://api.themoviedb.org/3/movie/%1/external_ids"
    ).arg(tmdbId);

    QByteArray urlData = url.toUtf8();

    curl_easy_setopt(hnd, CURLOPT_URL, urlData.constData());
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "accept: application/json"
    );

    headers = curl_slist_append(
        headers,
        "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJiMzQyOTNlNDA0NGQwM2I4MzQyOWJiNTE4Y2IzM2NlYiIsIm5iZiI6MTc4OTMzMDc5NS45MTY5OTk4LCJzdWIiOiI2YWE3MDU2YmZkODVjMjM0NDVjMTdjNDIiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0.HQgfX1GYfEB8Oyxg112EmUMUBKdsjKrgw3RxAWEEa_w"    );

    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);

    QString imdbId;

    if (ret == CURLE_OK) {
        QJsonParseError error;

        QJsonDocument json =
            QJsonDocument::fromJson(response, &error);

        if (error.error == QJsonParseError::NoError &&
            json.isObject()) {
            qDebug() << json.object();

            imdbId = json.object()["imdb_id"].toString();
            }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);

    return imdbId;
}


void SyncManager::getPopularMovies() {
    QString job ="popularMovies-themoviedb";
    if (!shouldFetchToday(job)) {
        return;
    }

    //setup the curl
    CURL *hnd = curl_easy_init();
    QByteArray response;
    curl_easy_setopt(
       hnd,
       CURLOPT_URL,
       "https://api.themoviedb.org/3/movie/popular?language=en-US&page=1"
   );
    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEFUNCTION,
        writeCallback
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEDATA,
        &response
    );

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "accept: application/json"
    );

    headers = curl_slist_append(
        headers,
        "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJiMzQyOTNlNDA0NGQwM2I4MzQyOWJiNTE4Y2IzM2NlYiIsIm5iZiI6MTc4OTMzMDc5NS45MTY5OTk4LCJzdWIiOiI2YWE3MDU2YmZkODVjMjM0NDVjMTdjNDIiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0.HQgfX1GYfEB8Oyxg112EmUMUBKdsjKrgw3RxAWEEa_w"
    );


    curl_easy_setopt(
        hnd,
        CURLOPT_HTTPHEADER,
        headers
    );


    CURLcode ret = curl_easy_perform(hnd);

    if (ret != CURLE_OK) {
        qDebug() << "CURL error:"
                 << curl_easy_strerror(ret);
    } else {
        qDebug() << response;
    }

    QJsonParseError error;

    QJsonDocument json =
        QJsonDocument::fromJson(response, &error);

    if (error.error != QJsonParseError::NoError ||
        !json.isObject()) {

        qDebug() << "Invalid JSON:"
                 << error.errorString();


        }

    QJsonObject data = json.object();

    qDebug() << "data['page']" << data["page"];
    qDebug() <<"data['results']"<< data["results"];

    QJsonArray results = data["results"].toArray();
    qDebug() << results.count();
    for(QJsonValueRef result : results) {
        //TODO : Likely a good idea, to parse each movie into a catalog,
        //and then have a insert Catalog funciton

        Movie movie = parseTMBDJsonMovie(result.toObject());
        movie.imdbID = getImdbId(result.toObject()["id"].toInt());

        databaseManager.insertMovie(movie);
    }


    databaseManager.insertFetch(job);
    //clean updatabaseManager

    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);
    qDebug() << "Finished";
}



void SyncManager::getTop10(const QString& country, const QString& service, const QString& content) {
    QString job = QString("motn_top10_%1_%2_%3")//Movie of the Night (MOTN)
                  .arg(country)
                  .arg(service)
                  .arg(content); //movie or show
    if (!shouldFetchToday(job)) {
        return;
    }
    //setup the curl
    CURL* hnd = curl_easy_init();

    if (!hnd) {
        return;
    }
    QString movieOfTheNightToken =
    QProcessEnvironment::systemEnvironment()
        .value("MOVIEOFTHENIGHT_API_KEY");
    QByteArray response;

    QString url =
        QString("https://api.movieofthenight.com/v4/shows/top"
                "?country=%1"
                "&service=%2"
                "&show_type=%3"
                "&output_language=%4")
            .arg(country, service, content, "en");

    QByteArray urlUtf8 = url.toUtf8();

    curl_easy_setopt(
        hnd,
        CURLOPT_URL,
        urlUtf8.constData()
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEFUNCTION,
        writeCallback
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_WRITEDATA,
        &response
    );

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "Accept: application/json"
    );
    QString authQString = "X-API-Key: " + movieOfTheNightToken;
    QByteArray auth_stringByte = authQString.toLocal8Bit();
    const char* auth_string = auth_stringByte.data();;
    headers = curl_slist_append(
        headers,
        auth_string
    );

    curl_easy_setopt(
        hnd,
        CURLOPT_HTTPHEADER,
        headers
    );

    CURLcode result = curl_easy_perform(hnd);

    if (result != CURLE_OK) {
        qDebug() << "MOTN request failed:"
                 << curl_easy_strerror(result);

        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);
        return;
    }


    QJsonParseError error;

    QJsonDocument responseJson = QJsonDocument::fromJson(response, &error);
    int placement = 0;
    QJsonArray movies;
    for (const QJsonValue value : responseJson.array()) {
        placement+=1;
        //this usually returns "movie/TMDBID" -> or show if it is a show
        QString tmdb = value["tmdbId"].toString().split("/").takeLast();
        qDebug() << tmdb;
        if (databaseManager.getMovie(tmdb).movies.isEmpty()) {
            Movie movie = getMovieTMDB(tmdb);
            qDebug() << movie.imdbID;
            databaseManager.insertMovie(movie);
        }
        QJsonObject entry;
        entry["tmdbId"] = tmdb;
        entry["placement"] = placement;

        movies.append(entry);
    }
    databaseManager.insertTopMovies(movies, service, country, content);

    /*
        Game plan!
        Parse this information (we need their index (placement on top 10) and their tmbd id
        We then check if we have a movie with that tbmd id, after which two things happens
            1. We have the movie.
                Then we do nothing
            2. We DONT have the movie. Then we get the movie from the tmbd database
                - Parse and add it to the datebase

        We add the ten movies (only tmdb and plcement) to the table with top10 movies
        which has tmdb, placement, service, country
    */
    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);


    databaseManager.insertFetch(job);
}


bool SyncManager::shouldFetchToday(const QString &job) {
    return true; //testing
    //https://www.w3schools.com/cpp/cpp_date.asp
    // Get the timestamp for the current date and time
    time_t timestamp;
    time(&timestamp); //format Fri Sep 25 16:09:12 2026
    struct tm datetime = *localtime(&timestamp);
    // Display the date and time represented by the timestamp
    //YYYY * 10000
    //month * 100
    //day * 1
    //would mean any previous days would always be lower than any higher. Also no duplicates
    //YYYYYYYY => YYYY0000, MMMM=> MM00, DD
    //YYYY0000 => YYYYMM00 => YYYYMMDD
    //check docs (they count time in years from 1900)
    int uniquedate  = (datetime.tm_year + 1900)*10000
                    + datetime.tm_mon * 100
                    + datetime.tm_mon;
    //We need to find the largest int in the api_fetch_log table, with the same job.

    int latestFetchDate = databaseManager.getFetchDate(job);
    if (uniquedate > latestFetchDate) {//only fetch once a day
        return true;
    }
    return false;
}

void SyncManager::insertFetch(const QString& job) {
    databaseManager.insertFetch(job);
}

