//
// Created by LHF on 02/09/2026.
//
#pragma once
#include <QLocalSocket>

class LocalClient : public QObject {
public:
    explicit LocalClient();
    void connectToCore();
protected:
    void sendMessage(QByteArrayView);
    void readMessage();
    virtual void handleMessage(QByteArrayView, QLocalSocket) = 0;

private:
    QLocalSocket socket;
    QByteArray readBuffer;
};








