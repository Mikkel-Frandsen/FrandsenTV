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

# 31/08/2026

Currently, working on getting the IPC layer formalized.
I will start with these commands:
STATUS
PLAY <movie>
PAUSE
STOP
and these events/responses: ONLINE
PLAYBACK_STARTED
PLAYBACK_STOPPED

## Small refactor
```plantuml
QObject::connect(&server,&QLocalServer::newConnection,
    [this]() {
        clientSocket = server.nextPendingConnection();
        qDebug() << "Client Connected to us!";
        //we get a segmentation fault without this check ;)
        if (clientSocket == nullptr) {
            qDebug() << "No client socket!";
            return;
        }
        QObject::connect(clientSocket,&QLocalSocket::readyRead,
        [this]() {
                qDebug() << "Message received!";
                readMessage();
            });
        });
```
I decided to extrapolate the lamdpa function with the connect (In CoreServer).

# 02/09/2026
I decided to rework the current workings, to ensure a proper design of these important protocols.

I will split the protocols into a different things;
    
Query: 
    From A->B->A, act based on the information in the query AND the state of B.
    Returns from data in B->A
    There are no side effects - so safe to retry

    Examples:
        getCatalog(), search(query), getSettings(), getAvailability(movie_id)
        
Command: 
    From A->B, this causes a state change or an external effect. 
    No data returned. Maybe events are sent

    Examples:
        play(), pause(), selectProvider(), setSettings(), launchGame()

Events:
    From B->A, annouces something has happened.

Ack:
    From B->A, annouces that B has received a command, and will now start to do it
    Note it does not garentuee it will be done! This is what events are used for

I have implemented this, and fairly happy with it.

I will now turn to implementing a very simple database manager. 
This will works as such:
```
     ┌──────────────┐
     │      UI      │
     └──────┬───────┘
            │ IPC
            ▼
     ┌──────────────┐
     │     Core     │
     │    Server    │
     └──────┬───────┘
            │ IPC
            ▼
┌─────────────────────────┐
│    DatabaseManager      │
│         Server          │
└───────────┬─────────────┘
            ▲
            │ IPC
            │
      ┌─────┴─────┐
      │   Sync    │
      │  Client   │
      └───────────┘
```
After reworking the protocol such that we have a database protocol and a core protocol, I will now start on the database server.
Both Sync and Core will eventually become DatabaseClients, connected to the DatabaseServer. 
The databaseServer will handle communication with clients, and forward requests to the databaseManager who will do the actual SQL

I here decided, that since we now have two servers, implementing the same IPC stuff could be done using inherientece. 
I decided to use the template method design pattern for this
