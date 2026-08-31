#include<QCoreApplication>
#include <QLocalServer>
#include <QDebug>

// For now this should create a localServer which listens on smartTV-core
// The UI will then send a request asking for its status (which will only be answered once ONLINE)

class Core {
    public:
        Core();
        bool isOnline();
    private:
        QLocalServer server;

};

Core::Core()
{
    if (!server.listen("smartTV-core")) {
        qWarning() << server.errorString();
    }
}

//Simple function for exposing whether the server is currently listening
bool Core::isOnline() {
    return server.isListening();
}


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    Core core;
    bool isListening = core.isOnline();
    qDebug() << isListening;

    return app.exec();


}

