//
// Created by LHF on 16/09/2026.
//

#include "PlaybackManager.h"
#include <QDebug>
#include <Player/MpvPlayer.h>
PlaybackManager::PlaybackManager(QObject *parent)
    : QObject(parent),
      player(new MpvPlayer())
{
    connect(
        &torrentio,
        &TorrentioProvider::streamsReady,
        this,
        &PlaybackManager::onStreamsReady
    );

    connect(
        &torrentio,
        &TorrentioProvider::streamsError,
        this,
        &PlaybackManager::onStreamsError
    );

    connect(
        &realDebrid,
        &RealDebridClient::streamReady,
        this,
        &PlaybackManager::onStreamReady
    );

    connect(
        &realDebrid,
        &RealDebridClient::streamError,
        this,
        &PlaybackManager::onStreamError
    );
}


void PlaybackManager::playMovie(const QString& stremioId)
{
    qDebug() << "play movie";
    torrentio.getStreams(stremioId);
    qDebug() << "After getStreams";

}

void PlaybackManager::onStreamsReady(const QList<Stream>& streams)
{
    qDebug() << "on Streams Ready";
    this->streams = streams;
    currentStream = 0;

    tryCurrentStream();

}
void PlaybackManager::tryCurrentStream()
{
    if (currentStream >= streams.size()) {
        qDebug() << "No more streams to try";
        return;
    }

    qDebug() << "Trying stream:" << currentStream;
    qDebug() << streams[currentStream].infoHash;

    realDebrid.getStreamLink(streams[currentStream].infoHash, streams[currentStream].fileIdx);
}

void PlaybackManager::onStreamsError(const QString& error)
{
}
void PlaybackManager::onStreamReady(const QString& stream)
{
    qDebug() << "on Stream (no S) Ready";

    bool playing = player->playMovie(stream);
    if (playing) {
        qDebug() << "playing()";
    }

}void PlaybackManager::onStreamError(const QString& error)
{
    //we should try the next stream here.
    currentStream+=1;
    tryCurrentStream();
}