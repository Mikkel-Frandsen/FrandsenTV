// Catalog.h

#pragma once

#include "movie.h"
#include <QList>
class Catalog
{
public:
    QString id;
    QString title;
    QList<Movie> movies;
};