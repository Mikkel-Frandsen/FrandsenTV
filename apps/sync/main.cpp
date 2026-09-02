#include <QCoreApplication>

#include "SyncManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SyncManager syncManager;
    syncManager.getPopularMovies();
    syncManager.getTop10("dk", "prime", "movie");
    syncManager.getTop10("dk", "netflix", "movie");
    syncManager.getTop10("dk", "disney", "movie");

    //qDebug() << syncManager.shouldFetchToday("popularMovies-themoviedb");


    return app.exec();
}
