#include <QApplication>
#include <QEventLoop>
#include <QTcpSocket>
#include "server.h"
#include "frame.h"
#include "common.h"


void Server::run()
{
    m_server = new QTcpServer();

    if(!m_server->listen(QHostAddress::LocalHost, PORT)) {
        qInfo() << "Server could not start.";
        return;
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(handleConnection()), Qt::DirectConnection);

    exec();  // Start event loop
}

void Server::handleConnection()
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

    Frame *newFrame = new Frame(reinterpret_cast<const uchar*>(byteArray.constData()));

    newFrame->moveToThread(QApplication::instance()->thread());  // Move to main thread

    QMetaObject::invokeMethod(newFrame, "finishInitInMainThread", Qt::QueuedConnection);
}




