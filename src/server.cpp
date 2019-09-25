#include <QApplication>
#include <QEventLoop>
#include "server.h"
#include "frame.h"
#include "common.h"


Server::Server(QObject *parent): QObject(parent)
{
    m_server = new QTcpServer(this);

    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!m_server->listen(QHostAddress::Any, PORT)) {
        qInfo() << "Server could not start.";
    }

}

void Server::newConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();

    QByteArray byteArray;
    while(true) {
        QByteArray buffer = socket->readAll();
        byteArray.append(buffer);

        if(socket->waitForReadyRead() == false) // If waitForReadyRead==false connection closed or error
            break;
    }

    socket->close();

    // When flipping an image of resolution less than 100x100 houdini only creates a header
    // packet and never sends the full pixel data. Detect this by checking the byteArray size.
    if(byteArray.size() < 1000)
        return;

    new Frame(reinterpret_cast<const uchar*>(byteArray.constData()));

    qApp->processEvents(); // Give the gui a chance to show the new frame before processing another
}

