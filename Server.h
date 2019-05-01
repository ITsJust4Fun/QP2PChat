#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void sendMessage(QJsonDocument &doc);
    void addNewUser(QJsonDocument &doc, QTcpSocket *socket);
    QTcpSocket * findSocket(QString name);
    virtual ~Server();

    QList<QTcpSocket *> sockets;
    QByteArray data;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    QJsonDocument doc;
    QJsonParseError docError;
    QMap<QString, QMap<QString, QList<QString>>> *users;
    QString localName = "J4F";

public slots:
    void startServer(quint16 port);
    void incomingConnection(int socketDescriptor);
    void socketReady();
    void socketDisconnect();
    void addMsgToDatabase(const QString &user, const QString &msg);
};

#endif // SERVER_H
