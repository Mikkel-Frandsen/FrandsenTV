//
// Created by LHF on 16/09/2026.
//

#pragma once
#include <QString>

class Player {
public:
    virtual ~Player() = default;

    virtual bool playMovie(const QString&) = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;


};
