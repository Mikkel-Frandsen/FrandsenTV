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


QStringList TMDBHeaders() {
    QString theMovieDBToken =
    QProcessEnvironment::systemEnvironment()
        .value("THEMOVIEDB_READ_ACCESS");

    QString authString ="Authorization: Bearer " +  theMovieDBToken;
    QString acceptHeader = "accept: application/json";
    QStringList headers;
    headers.append(acceptHeader);
    headers.append(authString);
    return headers;
}

Movie SyncManager::getMovieTMDB(const QString &tmbd_id) {
    //setup the curl
    QString url =
    "https://api.themoviedb.org/3/movie/" + tmbd_id;

    HttpResponse response = httpClient.get(url, TMDBHeaders());
    if (!response.networkSucces()) {
        qDebug() << "Get Movie TMDB CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Get  Movie TMDB HTTP request failed:"  << response.statusCode;
        return {};
    }


    QJsonObject data = httpClient.convertHttpResponseToJson(response);
    Movie movie = parseTMBDJsonMovie(data);
    qDebug() << movie.imdbID;

    qDebug() << "Finished";
    return movie;

}


QString SyncManager::getImdbId(int tmdbId)
{

    QString url = QString(
        "https://api.themoviedb.org/3/movie/%1/external_ids"
    ).arg(tmdbId);


    HttpResponse response = httpClient.get(url, TMDBHeaders());

    if (!response.networkSucces()) {
        qDebug() << "Get ImdbId CURL request failed:" << curl_easy_strerror(response.curlCode);
        return "";
    }
    if (!response.httpSucess()) {
        qDebug() << "Get ImdbId HTTP request failed:"  << response.statusCode;
        return "";
    }

    QString imdbId;
    QJsonObject json = httpClient.convertHttpResponseToJson(response);
    imdbId = json["imdb_id"].toString();


    return imdbId;
}


void SyncManager::getPopularMovies() {
    QString job ="popularMovies-themoviedb";
    if (!shouldFetchToday(job)) {
        return;
    }

    //URL
    QString url = "https://api.themoviedb.org/3/movie/popular?language=en-US&page=1";
    //HEADERS

    //GET RESPONSE
    HttpResponse response = HttpClient::get(url, TMDBHeaders());
    if (!response.networkSucces()) {
        qDebug() << "Get Popular Movies CURL request failed:" << curl_easy_strerror(response.curlCode);
        return;
    }
    if (!response.httpSucess()) {
        qDebug() << "Get Popular Movies HTTP request failed:"  << response.statusCode;
        return;
    }
    QJsonObject data = httpClient.convertHttpResponseToJson(response);

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

    QString movieOfTheNightToken =
    QProcessEnvironment::systemEnvironment()
        .value("MOVIEOFTHENIGHT_API_KEY");

    QString url =
        QString("https://api.movieofthenight.com/v4/shows/top"
                "?country=%1"
                "&service=%2"
                "&show_type=%3"
                "&output_language=%4")
            .arg(country, service, content, "en");
    QStringList headers;
    headers.append("Accept: application/json");
    QString authQString = "X-API-Key: " + movieOfTheNightToken;
    headers.append(authQString);

    HttpResponse response = httpClient.get(url, headers);
    if (!response.networkSucces()) {
        qDebug() << "Get top 10 CURL request failed:" << curl_easy_strerror(response.curlCode);
        return;
    }
    if (!response.httpSucess()) {
        qDebug() << "Get top 10 HTTP request failed:"  << response.statusCode;
        return;
    }


    QJsonObject json = httpClient.convertHttpResponseToJson(response);

    int placement = 0;
    QJsonArray movies;
    for (const QJsonValue value : json) {
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

