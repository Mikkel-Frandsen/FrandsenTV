//
// Created by LHF on 30/08/2026.
//

#ifndef SMARTTV_APPMODEL_H
#define SMARTTV_APPMODEL_H
#include <QObject>
#include <QString>


class AppModel : public QObject
{
    Q_OBJECT

public:
    explicit AppModel(QObject *parent = nullptr);

    Q_INVOKABLE void sayHello();

    signals:
        void messageChanged(QString message);

private:
    QString message;
};

#endif //SMARTTV_APPMODEL_H
