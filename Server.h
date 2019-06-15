#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>

#include "items/downloaditem.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void sendMessage(QJsonDocument &doc);
    void addNewUser(QJsonDocument &doc, QTcpSocket *socket);
    QTcpSocket * findSocket(QString name);
    static bool isJsonValid(QJsonDocument &doc, QJsonParseError &docError);
    void connectWithServerReq(const QString &ip);
    QStringList getMessagesFrom(const QString &user);
    QList<QString> changeSenderInMessages(QJsonArray &messages, const QString &user);
    virtual ~Server();

    QList<QTcpSocket *> sockets;
    QByteArray data;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    QJsonDocument doc;
    QJsonParseError docError;
    QMap<QString, QMap<QString, QList<QString>>> *users;
    QString localName = "";

public slots:
    void startServer(quint16 port);
    void incomingConnection(int socketDescriptor);
    void socketReady();
    void socketDisconnect();
    void addMsgToDatabase(const QString &user, const QString &msg);
    void setData(const QString &user, const QString &ip, const QString &mask);
    void sendUploadRequest(const QString &user, QList<DownloadItem *> files, qint64 size);
    void acceptUploadRequest(const QString &user);
    void rejectUploadRequest(const QString &user);

signals:
    void dataReady(const QString &user, const QString &ip, const QString &mask);
};

#endif // SERVER_H
