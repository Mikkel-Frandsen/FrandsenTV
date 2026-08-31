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
This can be seen in version 0.1.0.
#### Expanding the model
I have now implemented a simple connection between the client and server.
It allows for the client (once connected) to request the status of the server, and if the message is STATUS, the server responds with ONLINE. 
The client then also prints based on the answer received from the server.
Very simple and basic protocol.

The next step is to hook all this up to a GUI.
The idea is to create a button that will ask the server for its status, and then display the result. 

These ideas is what will be used to create the actual system.

Some refactoring **is** required once more functalities arrive. 

This was done, and works. the GUI is hooked up and works.
Currently only a button is there, which requests a status. This makes the client ask the server, for a status, and in return makes the client update the status - using a signal, the QML acts upon changing the text.

Next is to implement this as actual commands/requests so it is easier to add new ones.
This creates a clean seperation; The QML does not know about how the client/server works. It only knows the exposed methods.

# 30/08/2026

I have intially thought about the structure of this project for quite a long time, and have finially decided to concretise my thouhgts in this rep.
