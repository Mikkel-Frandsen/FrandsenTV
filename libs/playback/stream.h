//
// Created by LHF on 15/09/2026.
//

#pragma once

#include <QString>

struct Stream
{
    QString name;
    QString title;
    QString infoHash;
    int fileIdx;
    QString bingeGroup; //Holds information such as 4K, Blueray, REMUX etc.
};