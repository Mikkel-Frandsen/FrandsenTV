//
// Created by LHF on 01/10/2026.
//
#pragma once
#include <QString>

namespace Catagory {
    enum class Provider {
        Prime,
        Netflix,
        HBO,
        Disney,
        Apple,
    };

    struct CategoryDefinition
    {
        QString key;
        QString name;
        QString type;
        Provider provider;
        QString service;
        QString country;
    };
}
