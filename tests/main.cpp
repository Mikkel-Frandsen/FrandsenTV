#include <QCoreApplication>
#include "../libs/playback/Player/MpvPlayer.h"
#include <QDebug>

#include "PlaybackManager.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    PlaybackManager playbackManager;
    playbackManager.playMovie("tt33764258");

    return app.exec();
}
