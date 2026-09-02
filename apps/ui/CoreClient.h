//
// Created by LHF on 31/08/2026.
//

#ifndef SMARTTV_CORECLIENT_H
#define SMARTTV_CORECLIENT_H
#include <QLocalSocket>


class CoreClient : public QObject {
    Q_OBJECT
public:
    CoreClient();
    void connectToCore();

    //Accessable functions for the QML
    Q_INVOKABLE void getStatus();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();

private:
    QLocalSocket socket;

    QByteArray readBuffer;

    void handleAcknowledgment(QByteArrayView);
    void handleEvent(QByteArrayView);

    void sendMessage(QByteArrayView);
    void readMessage();



signals:
    void statusChanged(bool online);
    void playbackPaused();
    void playbackStopped();

};



#endif //SMARTTV_CORECLIENT_H
