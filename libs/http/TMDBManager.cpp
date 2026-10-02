//
// Created by LHF on 28/09/2026.
//

#include "TMDBManager.h"

#include <QJsonArray>

TMDBManager::TMDBManager() {

}

QStringList TMDBManager::TMDBHeaders() {
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

QString TMDBManager::getImdbId(QString tmdbId)
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

Movie TMDBManager::parseTMBDJsonMovie(QJsonObject json_movie) {

    Movie movie;
    int id = json_movie["id"].toInt();
    QString id_str = QString::number(id);
    movie.stremioID =  id_str;
    movie.title =  json_movie["original_title"].toString();
    movie.description = json_movie["overview"].toString();
    movie.imdbID = json_movie["imdb_id"].toString();
    movie.poster_path = json_movie["poster_path"].toString();

    return movie;
}

Catalog TMDBManager::getNewReleases() {
    QDate today = QDate::currentDate();
    QString gte = today.addDays(-30).toString("yyyy-MM-dd");
    QString lte = today.toString("yyyy-MM-dd");

    QString url =
    "https://api.themoviedb.org/3/discover/movie"
    "?region=US"
    "&with_release_type=2|3"
    "&release_date.gte=" + gte +
    "&release_date.lte=" + lte +
    "&sort_by=popularity.desc";
    QStringList headers = TMDBHeaders();
    HttpResponse response = httpClient.get(url, TMDBHeaders());
    if (!response.networkSucces()) {
        qDebug() << "Get Popular Movies CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Get Popular Movies HTTP request failed:"  << response.statusCode;
        return {};
    }
    QJsonObject data = httpClient.convertHttpResponseToJson(response);

    QJsonArray results = data["results"].toArray();

    Catalog catalog;
    catalog.title = "New Releases";
    for(QJsonValueRef result : results) {
        /*QString imdb = getImdbId(result.toObject()["id"].toInt());
        if (imdb != "") { //If there is no imdb ID, WE CANNOT find it
            Movie movie = parseTMBDJsonMovie(result.toObject());
            movie.imdbID = imdb;
            catalog.movies.append(movie);
        }*/
        Movie movie = parseTMBDJsonMovie(result.toObject());
        catalog.movies.append(movie);

    }
    return catalog;

}



Catalog TMDBManager::getPopularMovies() {
    //URL
    QString url = "https://api.themoviedb.org/3/movie/popular?language=en-US&page=1";
    //HEADERS
    //GET RESPONSE
    HttpResponse response = HttpClient::get(url, TMDBHeaders());
    if (!response.networkSucces()) {
        qDebug() << "Get Popular Movies CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Get Popular Movies HTTP request failed:"  << response.statusCode;
        return{};
    }
    QJsonObject data = httpClient.convertHttpResponseToJson(response);
    Catalog catalog;
    catalog.title = "Popular Movies";


    QJsonArray results = data["results"].toArray();
    qDebug() << results.count();
    for(QJsonValueRef result : results) {
        /*QString imdb = getImdbId(result.toObject()["id"].toInt());
        if (imdb != "") { //If there is no imdb ID, WE CANNOT find it
            Movie movie = parseTMBDJsonMovie(result.toObject());
            movie.imdbID = imdb;
            catalog.movies.append(movie);
        }*/
        Movie movie = parseTMBDJsonMovie(result.toObject());
        catalog.movies.append(movie);

    }
    return catalog;

}
int getTmdbGenreId(const QString &genreName) {
    static const QMap<QString, int> genreMap = {
        {"action", 28},
        {"adventure", 12},
        {"animation", 16},
        {"comedy", 35},
        {"crime", 80},
        {"documentary", 99},
        {"drama", 18},
        {"family", 10751},
        {"fantasy", 14},
        {"history", 36},
        {"horror", 27},
        {"music", 10402},
        {"mystery", 9648},
        {"romance", 10749},
        {"scifi", 878},
        {"science_fiction", 878},
        {"thriller", 53},
        {"war", 10752},
        {"western", 37}
    };

    return genreMap.value(genreName.toLower(), -1);
}

QString TMDBManager::convertGenresToIds(const QString &genreString) {

    // Handle single or pipe-delimited genre names ("comedy|action")
    QStringList names = genreString.split('|', Qt::SkipEmptyParts);
    QStringList ids;

    for (const QString &name : names) {
        QString key = name.trimmed().toLower();
        int genreId = getTmdbGenreId(key);
        if (genreId != -1) {
            ids.append(QString::number(genreId));
        }
    }

    return ids.join(",");
}

Catalog TMDBManager::fetchMoviesByGenre(const QString &genreString, const QString &country) {
    Catalog catalog;
    QString genreIds = convertGenresToIds(genreString);
    if (genreIds.isEmpty()) {
        qWarning() << "No valid TMDB genre IDs mapped from input:" << genreString;
        return catalog;
    }
    //URL
    QString url = QString("https://api.themoviedb.org/3/discover/movie"
                          "?with_genres=%1"
                          "&sort_by=popularity.desc"
                          "&region=%2"
                          "&language=en-US"
                          "&page=1")
                      .arg(genreIds, country);
    //HEADERS
    //GET RESPONSE
    HttpResponse response = HttpClient::get(url, TMDBHeaders());
    if (!response.networkSucces()) {
        qDebug() << "Get Movies on Genre CURL request failed:" << curl_easy_strerror(response.curlCode);
        return {};
    }
    if (!response.httpSucess()) {
        qDebug() << "Get Movies on Genre HTTP request failed:"  << response.statusCode;
        return{};
    }
    QJsonObject data = httpClient.convertHttpResponseToJson(response);

    catalog.title = genreString;


    QJsonArray results = data["results"].toArray();
    for(QJsonValueRef result : results) {
        /*QString imdb = getImdbId(result.toObject()["id"].toInt());
        if (imdb != "") { //If there is no imdb ID, WE CANNOT find it
            Movie movie = parseTMBDJsonMovie(result.toObject());
            movie.imdbID = imdb;
            catalog.movies.append(movie);
        }*/
        catalog.movies.append(parseTMBDJsonMovie(result.toObject()));

    }
    return catalog;

}