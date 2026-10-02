#pragma once

#include <QVector>

#include "Catalog.h"
#include "CatalogRep.h"
#include "DatabaseManager.h"
#include "TMDBManager.h"
#include "protocol.h"



class ContentManager {
public:
    explicit ContentManager(CatalogRep *catalogRep);

    QVector<Catalog> getPage(Protocol::Page page);
    QVector<Catalog> getHome();
    QString getImdbId(const QString &tmdbId);

private:
    CatalogRep *catalogRep; // Uses repository instead of raw API manager
};
