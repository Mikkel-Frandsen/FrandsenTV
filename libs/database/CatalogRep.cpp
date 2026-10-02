#include "CatalogRep.h"
#include <QDate>
#include <QDebug>

CatalogRep::CatalogRep(DatabaseManager *dbManager, TMDBManager *tmdbManager)
    : m_dbManager(dbManager), m_tmdbManager(tmdbManager) {}

bool CatalogRep::isCacheFresh(const QDateTime &lastFetched) const {
    return lastFetched.isValid() && lastFetched.date() == QDate::currentDate();
}



Catalog CatalogRep::getCatalog(const QString &type, const QString &service, const QString &country) {
    CatalogRecord record;

    // 1. Check local database record
    bool inDb = m_dbManager->getCatalogRecord(type, service, country, record);

    // 2. CACHE HIT: Valid record fetched today -> return from DB
    if (inDb && isCacheFresh(record.lastFetched)) {
        Catalog catalog;
        catalog.id = QString::number(record.id);
        catalog.title = record.name;
        catalog.movies = m_dbManager->getMoviesForCatalog(record.id);
        return catalog;
    }

    // 3. CACHE MISS / EXPIRED: Fetch fresh data from TMDB API
    qDebug() << "Cache miss or expired for catalog:" << type << ". Fetching from TMDB...";
    Catalog freshCatalog = fetchFromAPI(type, service, country);

    if (freshCatalog.movies.isEmpty()) {
        qWarning() << "API returned empty catalog or failed.";
        
        // Fallback: If API fails but we have stale cache, return stale cache
        if (inDb) {
            Catalog catalog;
            catalog.id = QString::number(record.id);
            catalog.title = record.name;
            catalog.movies = m_dbManager->getMoviesForCatalog(record.id);
            return catalog;
        }
        return {};
    }

    // 4. Save API results into SQLite cache
    int catalogId = m_dbManager->saveCatalogRecord(freshCatalog.title, type, service, country);
    if (catalogId != -1) {
        m_dbManager->setCatalogMovies(catalogId, freshCatalog.movies);
        freshCatalog.id = QString::number(catalogId);
    }

    return freshCatalog;
}



Catalog CatalogRep::fetchFromAPI(const QString &type, const QString &service, const QString &country) {
    // Route API request based on requested category type
    if (type == "new_releases") {
        return m_tmdbManager->getNewReleases();
    }
    if (type == "popular") {
        return m_tmdbManager->getPopularMovies();
    }
    if (type.contains("genre")) {
        QStringList genresList = type.split('_', Qt::SkipEmptyParts);
        QString genres = genresList.back(); //lose the genre in front of the actual genres
        return m_tmdbManager->fetchMoviesByGenre(genres, "US");

    }


    return {};
}

QString CatalogRep::getImdbId(const QString &tmdbId) {
    // 1. Check local DB
    QString cachedImdbId = m_dbManager->getImdbIdForMovie(tmdbId);
    if (!cachedImdbId.isEmpty()) {
        return cachedImdbId;
    }

    // 2. Fetch from TMDB API if not in DB
    QString freshImdbId = m_tmdbManager->getImdbId(tmdbId);

    // 3. Save to DB for future lookups
    if (!freshImdbId.isEmpty()) {
        m_dbManager->updateMovieImdbId(tmdbId, freshImdbId);
    }

    return freshImdbId;
}