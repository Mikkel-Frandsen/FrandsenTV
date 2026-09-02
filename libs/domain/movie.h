// Movie.h

#pragma once

#include <QString>

class Movie
{
public:
    QString title;
    QString description;
    QString poster_path;
    int year = 0;
    QString genre;
    QString stremioID; //Essentially TMBD's ID
    QString imdbID;
};