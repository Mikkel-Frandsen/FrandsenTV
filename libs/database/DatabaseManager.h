//
// Created by LHF on 13/09/2026.
//

# pragma once
#include <QtSql/QSqlDatabase>

#include "catalog.h"
#include "movie.h"
class DatabaseManager {

public:
    DatabaseManager();

    void insertTopMovies(const QJsonArray &movies,
                         const QString &service, const QString &country, const QString &content);

    Catalog getMovie(QString id);

    Catalog getTopMovies(const QString &service, const QString &country);

    void insertMovie(const Movie& movie);
    Catalog getCatalog();
    int getFetchDate(const QString& job);

    void insertFetch(const QString &job);

private:
    QSqlDatabase database;

};


