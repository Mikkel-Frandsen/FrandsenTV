//
// Created by LHF on 13/09/2026.
//

# pragma once
#include <qdatetime.h>
#include <QtSql/QSqlDatabase>

#include "Catalog.h"
#include "movie.h"

struct CatalogRecord {
    int id;
    QString name;
    QString type;
    QString service;
    QString country;
    QDateTime lastFetched;
};

class DatabaseManager {

public:
    DatabaseManager();

    void insertTopMovies(const QJsonArray &movies,
                         const QString &service, const QString &country, const QString &content);
    void insertCatalog(const Catalog);
    Catalog getCategory();
    Catalog getMovie(QString id);

    Catalog getTopMovies(const QString &service, const QString &country);

    Catalog getCatalog();
    int getFetchDate(const QString& job);

    void insertFetch(const QString &job);

    // --- Catalog Schema Operations ---
    bool getCatalogRecord(const QString &type, const QString &service, const QString &country, CatalogRecord &outRecord);
    int insertCatalogRecord(const QString &name, const QString &type, const QString &service, const QString &country);

    // --- Relation Operations ---
    QList<Movie> getMoviesForCatalog(int catalogId);
    bool setCatalogMovies(int catalogId, const QList<Movie> &movies);

    // --- Core Entity Operations ---
    void insertMovie(const Movie &movie);
    Movie getMovieById(const QString &stremioId);

    //Lookup functions
    void updateMovieImdbId(const QString &stremioId, const QString &imdbId);
    QString getImdbIdForMovie(const QString &stremioId);

    int saveCatalogRecord(
        const QString &name,
        const QString &type,
        const QString &service,
        const QString &country);
private:
    int currentUniqueDate();
    QSqlDatabase database;

};


