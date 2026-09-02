#pragma once


#include "protocol.h"

namespace CoreProtocol {
    namespace Query {
        constexpr QByteArrayView GetStatus = "GET_STATUS\n";
    }
    namespace Command {
        constexpr QByteArrayView Pause = "PAUSE\n";
        constexpr QByteArrayView Stop = "STOP\n";
    }
    // ACK = command was received and accepted.
    namespace Acknowledgment {
        constexpr QByteArrayView AckPause = "PAUSE_ACK\n";
        constexpr QByteArrayView AckStop = "STOP_ACK\n";
    }
    // Event = resulting state change actually occurred.
    namespace Event {
        constexpr QByteArrayView Online = "ONLINE\n";
        constexpr QByteArrayView PlaybackPaused = "PLAYBACK_PAUSED\n";
        constexpr QByteArrayView PlaybackStopped = "PLAYBACK_STOPPED\n";
    }

    Protocol::MessageType getMessageType(const QByteArray& message);
}

