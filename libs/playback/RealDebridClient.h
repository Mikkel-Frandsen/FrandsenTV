#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;

class RealDebridClient : public QObject
{
    Q_OBJECT

public:
    explicit RealDebridClient(QObject* parent = nullptr);

    void getStreamLink(const QString& infoHash, int fileIdx);

    signals:
        void streamReady(const QString& url);
        void streamError(const QString& error);

private:
    QString buildMagnetLink(const QString& infoHash);
    QString getRealDebridId(const QString& infoHash, const QString& authString);
    QJsonObject getRealDebridInfo(const QString& id, const QString& authString);
    void selectFileId(const QString& id, const QString& authString, int fileId);
    int getIdForSelection(QJsonObject info, const QStringList& fileFormat);
    QString getUnrestrictedLink(const QString& authString, const QString& link);

    QNetworkAccessManager* networkManager;
};