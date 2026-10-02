//
// Created by LHF on 28/09/2026.
//

#include "ContentManager.h"

ContentManager::ContentManager(CatalogRep *catalogRep)
    : catalogRep(catalogRep) {}

QVector<Catalog> ContentManager::getPage(Protocol::Page) {
    return getHome();

}

QVector<Catalog> ContentManager::getHome() {
    QVector<Catalog> rows;

    // Rep checks SQLite first; fetches TMDB only if stale/missing
    Catalog newReleases  = catalogRep->getCatalog("new_releases", "all", "US");
    Catalog popularMovies = catalogRep->getCatalog("popular", "all", "US");
    Catalog comedyActionMovies = catalogRep->getCatalog("genre_comedy|action","all", "US");
    rows.append(newReleases);
    rows.append(popularMovies);
    rows.append(comedyActionMovies);
    return rows;
}

QString ContentManager::getImdbId(const QString &tmdbId) {
    // Sanitize/validate input: check if tmdbId is a valid positive integer
    bool ok = false;
    int id = tmdbId.toInt(&ok);

    if (!ok || id <= 0) {
        qWarning() << "Invalid TMDB ID provided:" << tmdbId;
        return QString();
    }

    qDebug() << "Fetching IMDb ID for TMDB ID:" << tmdbId;
    QString imdbId = catalogRep->getImdbId(tmdbId);
    qDebug() << "Found IMDb ID:" << imdbId;

    return imdbId;
}
