#pragma once

#include "DatabaseManager.h"
#include "TMDBManager.h"
#include "Catalog.h"
#include <QString>

class CatalogRep {
public:
    CatalogRep(DatabaseManager *dbManager, TMDBManager *tmdbManager);

    // Primary entry point for fetching a catalog
    Catalog getCatalog(const QString &type, 
                       const QString &service = "all", 
                       const QString &country = "US");
    QString getImdbId(const QString &tmdbId);
private:
    DatabaseManager *m_dbManager;
    TMDBManager *m_tmdbManager;

    bool isCacheFresh(const QDateTime &lastFetched) const;
    Catalog fetchFromAPI(const QString &type, const QString &service, const QString &country);
};