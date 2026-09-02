#pragma once

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include "../domain/catalog.h"

namespace Protocol {

    enum class MessageType {
        CommandPause,
        CommandPlay,
        CommandStop,

        //The Idea is that the a client sends a query, which the server then answer with a response

        QueryGetCatalog,
        QueryGetMovie,
        QueryPing,
        QueryGetTopContent,


        ResponseGetCatalog,
        ResponsePing,
        ResponseGetTopContent,

        // responses/events later
        // ...
        
        Unknown
    };

    struct Message {
        MessageType type;
        QJsonObject data;
    };
    //Convert from IPC to Wire
    QByteArray encode(const Message& message);
    Message decode(const QByteArray& message);

    //Convert from Domain to IPC
    QJsonObject encodeCatalog(const Catalog&);
    Catalog decodeCatalog(const QJsonObject&);
}