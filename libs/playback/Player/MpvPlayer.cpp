#include "MpvPlayer.h"

#include <mpv/client.h>
#include <QDebug>

MpvPlayer::MpvPlayer()
{
    mpv = mpv_create();

    if (!mpv) {
        qDebug() << "Failed to create mpv player";
        return;
    }

    // Let mpv create and manage its own window.
    //mpv_set_option_string(mpv, "force-window", "yes");
    // actually enable bindings
    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    mpv_set_option_string(mpv, "alang", "en");
    mpv_set_option_string(mpv, "slang", "en");
    // Use GPU video output.
    mpv_set_option_string(mpv, "vo", "gpu");

    // Start fullscreen.
    mpv_set_option_string(mpv, "fs", "yes");

    // Better seeking.
    mpv_set_option_string(mpv, "hr-seek", "yes");

    // Show OSD.
    mpv_set_option_string(mpv, "osd-level", "5");

    // Show stats overlay.
    mpv_set_option_string(mpv, "load-stats-overlay", "yes");

  
    if (mpv_initialize(mpv) < 0) {
        qDebug() << "Failed to initialize mpv";

        mpv_terminate_destroy(mpv);
        mpv = nullptr;
        return;
    }

    qDebug() << "mpv initialized";
}

MpvPlayer::~MpvPlayer()
{
    if (mpv) {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }
}

bool MpvPlayer::playMovie(const QString& url)
{
    if (!mpv) {
        return false;
    }

    QByteArray urlData = url.toUtf8();

    const char* command[] = {
        "loadfile",
        urlData.constData(),
        nullptr
    };

    int result = mpv_command(mpv, command);

    if (result < 0) {
        qDebug() << "mpv loadfile failed:" << mpv_error_string(result);
        return false;
    }

    return true;
}

void MpvPlayer::pause()
{
    if (!mpv)
        return;

    const char* command[] = {
        "set",
        "pause",
        "yes",
        nullptr
    };

    mpv_command(mpv, command);
}

void MpvPlayer::resume()
{
    if (!mpv)
        return;

    const char* command[] = {
        "set",
        "pause",
        "no",
        nullptr
    };

    mpv_command(mpv, command);
}

void MpvPlayer::stop()
{
    if (!mpv)
        return;

    const char* command[] = {
        "stop",
        nullptr
    };

    mpv_command(mpv, command);
}
