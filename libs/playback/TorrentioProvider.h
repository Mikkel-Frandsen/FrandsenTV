#pragma once

#include <QObject>
#include <QList>
#include <QString>

#include "stream.h"

class QNetworkAccessManager;
class QJsonObject;

class TorrentioProvider : public QObject
{
    Q_OBJECT

public:
    explicit TorrentioProvider(QObject* parent = nullptr);
    void getStreams(const QString& stremioId);

signals:
    void streamsReady(const QList<Stream>& streams);
    void streamsError(const QString& error);

private:
    QList<Stream> jsonToStreams(const QJsonObject& json);

    QNetworkAccessManager* networkManager;
};