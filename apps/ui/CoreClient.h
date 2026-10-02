//
// Created by LHF on 31/08/2026.
//

#ifndef SMARTTV_CORECLIENT_H
#define SMARTTV_CORECLIENT_H
#include <QLocalSocket>
#include "localClient.h"
#include "protocol.h"
class CoreClient : public LocalClient {
    Q_OBJECT
public:
    CoreClient();

    //Accessable functions for the QML
    Q_INVOKABLE void ping();

    Q_INVOKABLE void getCatalog();

    Q_INVOKABLE void getTopContent(QString content, QString service, QString country);
    Q_INVOKABLE void getPage(Protocol::Page page);

    Q_INVOKABLE void play(QString streamio_id);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
protected:
    void handleMessage(Protocol::Message message) override;
private:

    void handleAcknowledgment(QByteArrayView);
    void handleEvent(QByteArrayView);






signals:
    void statusChanged(bool online);
    void playbackPaused();
    void playbackStopped();
    void catalogReceived(QVariantList movies);
    void topContentReceived(QVariantList movies, QString service, QString country, QString content);
    void pageReceived(QVariantList catalogs, int page);

};



#endif //SMARTTV_CORECLIENT_H
