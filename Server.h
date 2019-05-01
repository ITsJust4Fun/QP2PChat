#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    virtual ~Server();

    QList<QTcpSocket *> sockets;
    QByteArray Data;

public slots:
    void startServer(quint16 port);
    void incomingConnection(int socketDescriptor);
    void socketReady();
    void socketDisconnect();
};

#endif // SERVER_H
