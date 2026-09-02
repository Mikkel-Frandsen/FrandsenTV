
#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QtSql/QSqlQuery>


DatabaseManager::DatabaseManager()
{
    database = QSqlDatabase::addDatabase("QSQLITE");

    database.setDatabaseName(
        "/home/LHF/CLionProjects/FrandsenTV/database/entertainment"
    );

    if (!database.open()) {
        qDebug() << "Failed to open database:";
        QString err = database.lastError().text();
        qDebug() << err;
        return;
    }

    qDebug() << "Database opened successfully:"
             << database.databaseName();
}



void DatabaseManager::insertTopMovies(
    const QJsonArray& movies,
    const QString& service,
    const QString& country,
    const QString& content
) {
    // Start transaction
    if (!database.transaction()) {
        qWarning() << "Failed to start transaction:"
                   << database.lastError().text();
        return;
    }

    QSqlQuery query(database);

    // remove existing chart for this service/country/content (we are only in here if we have not done it today)


    query.prepare(R"(
        DELETE FROM top_movies
        WHERE service = :service
          AND country = :country
          AND content = :content
    )");

    query.bindValue(":service", service);
    query.bindValue(":country", country);
    query.bindValue(":content", content);

    if (!query.exec()) {
        qWarning() << "Failed to clear top movies:"
                   << query.lastError().text();

        database.rollback();
        return;
    }

    query.prepare(R"(
        INSERT INTO top_movies
            (tmdb_id, service, country, placement, content)
        VALUES
            (:tmdb_id, :service, :country, :placement, :content)
    )");


    for (const QJsonValueConstRef& value : movies) {

        QJsonObject movie = value.toObject();

        QString tmdbId = movie["tmdbId"].toString();
        int placement = movie["placement"].toInt();

        qDebug() << "Inserting top movie:"
                 << "TMDB:" << tmdbId
                 << "Placement:" << placement
                 << "Service:" << service
                 << "Country:" << country
                 << "Content:" << content;

        query.bindValue(":tmdb_id", tmdbId);
        query.bindValue(":service", service);
        query.bindValue(":country", country);
        query.bindValue(":placement", placement);
        query.bindValue(":content", content);

        if (!query.exec()) {
            qWarning() << "Failed inserting top movie:"
                       << query.lastError().text();

            database.rollback();
            return;
        }
    }

    if (!database.commit()) {
        qWarning() << "Failed to commit top movies:"
                   << database.lastError().text();

        database.rollback();
        return;
    }

    qDebug() << "Successfully updated top movies:"
             << service
             << country
             << content;
}
void DatabaseManager::insertMovie(const Movie& movie)
{
    QSqlQuery query(database);

    query.prepare(R"(
        INSERT INTO movie (
            stremio_id,
            title,
            description,
            poster,
            year,
            genre,
            imdb_id
        )
        VALUES (?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(stremio_id)
        DO UPDATE SET
            title = excluded.title,
            description = excluded.description,
            poster = excluded.poster,
            year = excluded.year,
            genre = excluded.genre,
            imdb_id = excluded.imdb_id
    )");

    query.addBindValue(movie.stremioID);
    query.addBindValue(movie.title);
    query.addBindValue(movie.description);
    query.addBindValue(movie.poster_path);
    query.addBindValue(movie.year);
    query.addBindValue(movie.genre);
    query.addBindValue(movie.imdbID);

    if (!query.exec()) {
        qDebug() << "Failed to insert movie:"
                 << query.lastError().text();
        qDebug() << "Executed query:" << query.lastQuery();
    }
}

Catalog DatabaseManager::getMovie(QString id)
{
    Catalog catalog;

    QSqlQuery query(database);

    query.prepare(R"(
        SELECT stremio_id, title, description, poster, year, genre, imdb_id
        FROM movie
        WHERE stremio_id = ?
    )");

    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Failed to get movie:"
                 << query.lastError().text();
        return catalog;
    }

    if (query.next()) {
        Movie movie;

        movie.stremioID = query.value("stremio_id").toString();
        movie.title = query.value("title").toString();
        movie.description = query.value("description").toString();
        movie.poster_path = query.value("poster").toString();
        movie.year = query.value("year").toInt();
        movie.genre = query.value("genre").toString();
        movie.imdbID = query.value("imdb_id").toString();

        catalog.movies.append(movie);
    }

    return catalog;
}


Catalog DatabaseManager::getCatalog() {
    Catalog catalog;

    QSqlQuery query(database);
    query.prepare(R"(
        SELECT stremio_id, title, description, poster, year, genre, imdb_id
        FROM movie
    )");

    if (!query.exec()) {
        qDebug() << "Failed to get movie:"
                 << query.lastError().text();
        return catalog;
    }
    while (query.next()) {
        Movie movie;

        movie.stremioID = query.value("stremio_id").toString();
        movie.title = query.value("title").toString();
        movie.description = query.value("description").toString();
        movie.poster_path = query.value("poster").toString();
        movie.year = query.value("year").toInt();
        movie.genre = query.value("genre").toString();
        movie.imdbID = query.value("imdb_id").toString();

        catalog.movies.append(movie);
    }
    return catalog;
}

Catalog DatabaseManager::getTopMovies(
    const QString& service,
    const QString& country)
{
    Catalog catalog;

    QSqlQuery query(database);

    query.prepare(R"(
        SELECT
            m.stremio_id,
            m.title,
            m.description,
            m.poster,
            m.year,
            m.genre,
            m.imdb_id
        FROM movie m
        JOIN top_movies tm
            ON m.stremio_id = tm.tmdb_id
        WHERE tm.service = ?
          AND tm.country = ?
        ORDER BY tm.placement
    )");

    query.addBindValue(service);
    query.addBindValue(country);

    if (!query.exec()) {
        qDebug() << "Failed to get top movies:"
                 << query.lastError().text();
        return catalog;
    }

    while (query.next()) {
        Movie movie;

        movie.stremioID = query.value("stremio_id").toString();
        movie.title      = query.value("title").toString();
        movie.description = query.value("description").toString();
        movie.poster_path      = query.value("poster").toString();
        movie.year       = query.value("year").toInt();
        movie.genre      = query.value("genre").toString();
        movie.imdbID    = query.value("imdb_id").toString();

        catalog.movies.push_back(movie);
    }

    return catalog;
}

int DatabaseManager::getFetchDate(const QString &job) {
    QSqlQuery query(database);

    query.prepare(R"(
        SELECT fetch_date
        FROM api_fetch_log
        WHERE job = ?
    )");
    query.addBindValue(job);
    if (!query.exec()) {
        qDebug() << "Failed to get latests fetch date:"
                 << query.lastError().text();
        return -1;
    }
    if (query.next()) {
        return query.value("fetch_date").toInt();
    }
    return -1;
}

void DatabaseManager::insertFetch(const QString &job) {
    QSqlQuery query(database);

    query.prepare(R"(
        INSERT INTO api_fetch_log (
            fetch_date,
            job
        )
        VALUES (?, ?)
    )");
    time_t timestamp;
    time(&timestamp); //format Fri Sep 25 16:09:12 2026
    struct tm datetime = *localtime(&timestamp);
    int uniquedate  = (datetime.tm_year + 1900)*10000
                    + datetime.tm_mon * 100
                    + datetime.tm_mon;

    query.addBindValue(uniquedate);
    query.addBindValue(job);
    if (!query.exec()) {
        qDebug() << "Failed to insert api fetch log:"
                 << query.lastError().text();
        qDebug() << "Executed query:" << query.lastQuery();
    }
}