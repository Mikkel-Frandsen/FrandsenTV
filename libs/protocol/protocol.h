#pragma once

#include <QByteArrayView>
#include <QByteArray>

namespace Protocol {

    enum class MessageType {
        Query,
        Command,
        Acknowledgment,
        Event,
        Unknown
    };


}