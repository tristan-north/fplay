#ifndef SERVER_H
#define SERVER_H

#include <QDebug>
#include <QTcpServer>
#include <QThread>
#include "frame.h"

class Server : public QThread
{
    Q_OBJECT

public:
    Server(QObject *parent) : QThread(parent){}
    void run() override;

public slots:
    void handleConnection();

private:
    QTcpServer *m_server;

};
/*

class HandleConnectionThread : public QThread
{
    Q_OBJECT
public:
    HandleConnectionThread(qintptr socketDescriptor) : m_socketDescriptor(socketDescriptor){}
    void run() override;

private:
    qintptr m_socketDescriptor;

};
*/
#endif // SERVER_H
