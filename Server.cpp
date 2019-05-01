#include "Server.h"

Server::Server()
{

}

void Server::startServer(quint16 port)
{
    this->listen(QHostAddress::Any, port);
}

void Server::incomingConnection(int socketDescriptor)
{
    sockets.append(new QTcpSocket(this));
    QTcpSocket *socket = sockets.last();
    socket->setSocketDescriptor(socketDescriptor);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));

    qDebug() << "connected " + QString::number(socketDescriptor);
}

void Server::socketReady()
{

}

void Server::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    socket->deleteLater();
}

Server::~Server()
{

}
