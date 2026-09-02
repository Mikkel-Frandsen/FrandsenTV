//
// Created by LHF on 16/09/2026.
//

#pragma once


#include "RealDebridClient.h"
#include "TorrentioProvider.h"
#include "Player/MpvPlayer.h"
class PlaybackManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaybackManager(QObject* parent = nullptr);

    void playMovie(const QString& stremioId);

private slots:
    void onStreamsReady(const QList<Stream>& streams);
    void onStreamsError(const QString& error);
    void onStreamReady(const QString& stream);
    void onStreamError(const QString& error);
private:
    TorrentioProvider torrentio;
    RealDebridClient realDebrid;
    MpvPlayer* player;
    int currentStream = 0;
    QList<Stream> streams;
    void tryCurrentStream();
};