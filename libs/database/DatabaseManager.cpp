
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


//NEW FUNCTIONS;

bool DatabaseManager::getCatalogRecord(
        const QString &type,
        const QString &service,
        const QString &country,
        CatalogRecord &outRecord) {

    QSqlQuery query(database);

    query.prepare(R"(
        SELECT id, name, type, service, country, last_fetched
        FROM catalog
        WHERE type = :type AND service = :service AND country = :country
    )");
    query.bindValue(":type", type);
    query.bindValue(":service", service);
    query.bindValue(":country", country);

    if (!query.exec() || !query.next()) {
        return false;
    }
    outRecord.id = query.value("id").toInt();
    outRecord.name = query.value("name").toString();
    outRecord.type = query.value("type").toString();
    outRecord.service = query.value("service").toString();
    outRecord.country = query.value("country").toString();
    outRecord.lastFetched = query.value("last_fetched").toDateTime();

    return true;
}


int DatabaseManager::insertCatalogRecord(
        const QString &name,
        const QString &type,
        const QString &service,
        const QString &country) {
    // Start transaction
    if (!database.transaction()) {
        qWarning() << "Failed to start transaction:"
                   << database.lastError().text();
        return false;
    }
    QSqlQuery query(database);

    query.prepare(R"(
        INSERT INTO catalog (name, type, service, country, last_fetched)
        VALUES (:name, :type, :service, :country, :last_fetched)
        ON CONFLICT(type, service, country) DO UPDATE SET
            name = excluded.name,
            last_fetched = excluded.last_fetched
    )");
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":service", service);
    query.bindValue(":country", country);
    query.bindValue(":last_fetched", QDateTime::currentDateTime());
    if (!query.exec()) {
        qDebug() << "Failed to insert/update catalog record:" << query.lastError().text();
        return -1;
    }
    int catalogId = query.lastInsertId().toInt();
    //If the query somehow did not work, we manually fetch the id
    if (catalogId == 0) {
        CatalogRecord existing;
        if (getCatalogRecord(type, service, country, existing)) {
            catalogId = existing.id;
        }
    }

    return catalogId;
}

QList<Movie> DatabaseManager::getMoviesForCatalog(int catalogId)
{
    QSqlQuery query(database);

    // Join catalog, catalog_movie, and movie in a single query
    query.prepare(R"(
        SELECT
            c.name AS catalog_name,
            m.stremio_id,
            m.title,
            m.description,
            m.poster,
            m.year,
            m.genre,
            m.imdb_id
        FROM catalog c
        JOIN catalog_movie cm ON c.id = cm.catalog_id
        JOIN movie m ON cm.stremio_id = m.stremio_id
        WHERE c.id = ?
        ORDER BY cm.position ASC
    )");
    query.addBindValue(catalogId);
    QList<Movie> catalog;

    if (!query.exec()) {
        qDebug() << "Failed to get catalog and movies for ID:" << catalogId << query.lastError().text();
        return catalog;
    }

    while (query.next()) {
        // Grab the catalog name from the first row

        Movie movie;
        movie.stremioID   = query.value("stremio_id").toString();
        movie.title       = query.value("title").toString();
        movie.description = query.value("description").toString();
        movie.poster_path = query.value("poster").toString();
        movie.year        = query.value("year").toInt();
        movie.genre       = query.value("genre").toString();
        movie.imdbID      = query.value("imdb_id").toString();

        catalog.append(movie);
    }

    return catalog;
}
bool DatabaseManager::setCatalogMovies(int catalogId, const QList<Movie> &movies) {
    // 1. Begin outer transaction
    if (!database.transaction()) {
        qWarning() << "Failed to start transaction:" << database.lastError().text();
        return false;
    }

    // 2. Clear old positions for this catalog
    QSqlQuery clearQuery(database);
    clearQuery.prepare("DELETE FROM catalog_movie WHERE catalog_id = ?");
    clearQuery.addBindValue(catalogId);

    if (!clearQuery.exec()) {
        qDebug() << "Failed to clear catalog_movie:" << clearQuery.lastError().text();
        database.rollback(); // ALWAYS rollback before early returning!
        return false;
    }

    // 3. Insert movies and position links
    QSqlQuery linkQuery(database);
    linkQuery.prepare(R"(
        INSERT INTO catalog_movie (catalog_id, stremio_id, position)
        VALUES (:catalog_id, :stremio_id, :position)
    )");

    for (int i = 0; i < movies.size(); ++i) {
        const Movie &movie = movies.at(i);

        insertMovie(movie); // Single execution query (no internal transaction calls)

        linkQuery.bindValue(":catalog_id", catalogId);
        linkQuery.bindValue(":stremio_id", movie.stremioID);
        linkQuery.bindValue(":position", i);

        if (!linkQuery.exec()) {
            qDebug() << "Failed to insert catalog_movie link:" << linkQuery.lastError().text();
            database.rollback(); // ALWAYS rollback on failure!
            return false;
        }
    }

    // 4. Commit transaction
    return database.commit();
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

Movie DatabaseManager::getMovieById(const QString &stremioId) {

    QSqlQuery query(database);

    query.prepare(R"(
        SELECT stremio_id, title, description, poster, year, genre, imdb_id
        FROM movie
        WHERE stremio_id = ?
    )");

    query.addBindValue(stremioId);
    Movie movie;

    if (!query.exec()) {
        qDebug() << "Failed to get movie:"
                 << query.lastError().text();
        return movie;
    }

    if (query.next()) {

        movie.stremioID = query.value("stremio_id").toString();
        movie.title = query.value("title").toString();
        movie.description = query.value("description").toString();
        movie.poster_path = query.value("poster").toString();
        movie.year = query.value("year").toInt();
        movie.genre = query.value("genre").toString();
        movie.imdbID = query.value("imdb_id").toString();

    }

    return movie;
}

// DatabaseManager.cpp
QString DatabaseManager::getImdbIdForMovie(const QString &stremioId) {
    QSqlQuery query(database);
    query.prepare("SELECT imdb_id FROM movie WHERE stremio_id = ?");
    query.addBindValue(stremioId);

    if (query.exec() && query.next()) {
        QString imdbId = query.value("imdb_id").toString();
        if (!imdbId.isEmpty()) {
            return imdbId; // Cache hit!
        }
    }
    return "";
}

void DatabaseManager::updateMovieImdbId(const QString &stremioId, const QString &imdbId) {
    QSqlQuery query(database);
    query.prepare("UPDATE movie SET imdb_id = ? WHERE stremio_id = ?");
    query.addBindValue(imdbId);
    query.addBindValue(stremioId);
    query.exec();
}
int DatabaseManager::saveCatalogRecord(
        const QString &name,
        const QString &type,
        const QString &service,
        const QString &country) {
    CatalogRecord existing;
    if (getCatalogRecord(type, service, country, existing)) {
        QSqlQuery updateQuery(database);
        updateQuery.prepare("UPDATE catalog SET name = ?, last_fetched = ? WHERE id = ?");
        updateQuery.addBindValue(name);
        updateQuery.addBindValue(QDateTime::currentDateTime());
        updateQuery.addBindValue(existing.id);

        if (!updateQuery.exec()) {
            qDebug() << "Failed to update catalog record:" << updateQuery.lastError().text();
            return -1;
        }
        return existing.id;
    }

    QSqlQuery insertQuery(database);
    insertQuery.prepare("INSERT INTO catalog (name, type, service, country, last_fetched) VALUES (?, ?, ?, ?, ?)");
    insertQuery.addBindValue(name);
    insertQuery.addBindValue(type);
    insertQuery.addBindValue(service);
    insertQuery.addBindValue(country);
    insertQuery.addBindValue(QDateTime::currentDateTime());

    if (!insertQuery.exec()) {
        qDebug() << "Failed to insert catalog record:" << insertQuery.lastError().text();
        return -1;
    }

    return insertQuery.lastInsertId().toInt();
}

//OLD FUNCTIONS; WILL BE REMOVED ONCE THE OTHERS WORK!
void DatabaseManager::insertTopMovies(
    const QJsonArray& movies,
    const QString& service,
    const QString& country,
    const QString& content) {
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


int DatabaseManager::currentUniqueDate() {
    QDate today = QDate::currentDate();
    return (today.year() * 10000) + (today.month() * 100) + today.day();
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
    int uniquedate = currentUniqueDate();

    query.addBindValue(uniquedate);
    query.addBindValue(job);
    if (!query.exec()) {
        qDebug() << "Failed to insert api fetch log:"
                 << query.lastError().text();
        qDebug() << "Executed query:" << query.lastQuery();
    }
}