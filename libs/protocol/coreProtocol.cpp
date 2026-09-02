#include <coreProtocol.h>

Protocol::MessageType CoreProtocol::getMessageType(const QByteArray& message)
{
    if (message == Query::GetStatus) {
        return Protocol::MessageType::Query;
    }

    if (message == Command::Pause ||
        message == Command::Stop) {
        return Protocol::MessageType::Command;
        }

    if (message == Acknowledgment::AckPause ||
        message == Acknowledgment::AckStop) {
        return Protocol::MessageType::Acknowledgment;
        }

    if (message == Event::Online ||
        message == Event::PlaybackPaused ||
        message == Event::PlaybackStopped) {
        return Protocol::MessageType::Event;
        }

    return Protocol::MessageType::Unknown;
}