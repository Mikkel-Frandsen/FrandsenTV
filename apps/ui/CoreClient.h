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
    Q_INVOKABLE void requestStatus();

private:
    QLocalSocket socket;
    void readMessage();

signals:
    void statusChanged(bool online);
};



#endif //SMARTTV_CORECLIENT_H
