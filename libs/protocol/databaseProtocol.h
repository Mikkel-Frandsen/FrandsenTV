//
// Created by LHF on 02/09/2026.
//

#pragma once

#include "protocol.h"
namespace DatabaseProtocol {
    namespace Query {

    }
    namespace Command {

    }
    // ACK = command was received and accepted.
    namespace Acknowledgment {

    }
    // Event = resulting state change actually occurred.
    namespace Event {

    }
    Protocol::MessageType getMessageType(const QByteArray& message);
}




