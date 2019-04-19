#include <QApplication>
#include "server.h"
#include "frame.h"

#define PORT 37381


Server::Server(QObject *parent): QObject(parent)
{
    m_server = new QTcpServer(this);

    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!m_server->listen(QHostAddress::Any, PORT)) {
        qInfo() << "Server could not start.";
    }
    else {
        qInfo() << "Server started.";
    }

}

void Server::newConnection()
{
    qInfo() << "New connection.";

    QTcpSocket *socket = m_server->nextPendingConnection();

    QByteArray byteArray;
    while(true) {
        QByteArray buffer = socket->readAll();
        byteArray.append(buffer);

        if(socket->waitForReadyRead() == false) // If waitForReadyRead==false connection closed or error
            break;
    }

    socket->close();

    new Frame(reinterpret_cast<const uchar*>(byteArray.constData()));
}

