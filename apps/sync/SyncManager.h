//
// Created by LHF on 13/09/2026.
//

#pragma once
#include "DatabaseManager.h"
#include <ctime> // Import the ctime library
class SyncManager {
public:
    void getPopularMovies();

    void getTop10(const QString &country, const QString &service, const QString &content);

    SyncManager();

    bool shouldFetchToday(const QString& job);//todo make private
    void insertFetch(const QString &job);

private:
    DatabaseManager databaseManager;

    Movie getMovieTMDB(const QString &tmbd_id);

    QString getImdbId(int);

private:
};



