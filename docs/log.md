# Devolop plan
## v1
Create the core and UI as quite simple applications. 
Create a sqllite database, with manually inserted movies.
Get the most basic of expericence to work.

### In depth
I'd first like to implement the most basic of inter-application communication
The core should start, and listen for the UI on a local socket. Once it gets a response, it should relay that information to us.

#### Experimenting
I started by creating a very simple app that communicates with a QML application.

```plantuml

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

AppModel::AppModel(QObject *parent) : QObject(parent) {

}

void AppModel::sayHello() {
    message = "Hello!";
    emit messageChanged(message);
}
```

```plantuml

#include "AppModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    AppModel model;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(
        "appModel",
        &model
    );

    engine.loadFromModule("SmartTV", "Main");

    if (engine.rootObjects().isEmpty())
        return 1;

    return app.exec();

}
```

This worked, and the process thought me the basics of sockets and servers. 

I then moved to creating the _core_ and the _ui_ applications, and connecting them using QML.

# 30/08/2026

I have intially thought about the structure of this project for quite a long time, and have finially decided to concretise my thouhgts in this rep.
