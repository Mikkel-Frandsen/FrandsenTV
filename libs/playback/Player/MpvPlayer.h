//
// Created by LHF on 16/09/2026.
//
#pragma once
#include "Player.h"

struct mpv_handle;

class MpvPlayer : public Player {
public:
    MpvPlayer();
    ~MpvPlayer() override;
    bool playMovie(const QString&) override;
    void pause() override;
    void resume() override;
    void stop() override;
private:
    mpv_handle* mpv = nullptr;
};




