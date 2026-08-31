#include "AppModel.h"

AppModel::AppModel(QObject *parent) : QObject(parent) {

}

void AppModel::sayHello() {
    message = "Hello!";
    emit messageChanged(message);
}
