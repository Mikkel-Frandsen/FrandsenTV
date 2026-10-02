#pragma once

#include <QList>
#include "Catalog.h"
#include <QDate>
#include <QProcessEnvironment>

#include "HttpClient.h"

class TMDBManager
{
public:
    TMDBManager();
    Catalog getTrendingMovies();
    Catalog getPopularMovies();
    Catalog getNewReleases();

    //Helpers
    QStringList TMDBHeaders();

    Catalog fetchMoviesByGenre(const QString &genreString, const QString &country = "US");

    QString getImdbId(QString);

private:
    Movie parseTMBDJsonMovie(QJsonObject);
    QString convertGenresToIds(const QString &genreString);
    HttpClient httpClient;
};
