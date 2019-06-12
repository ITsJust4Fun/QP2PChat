#ifndef UPLOADER_H
#define UPLOADER_H

#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonParseError>

#include "server.h"

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = nullptr);
    virtual ~Uploader();

    void setPath(const QString &path);
    QString getPath() const;
    void setIp(const QString &ip);
    QString getIp() const;
    void uploadFile() const;
    void connectToServer();

private:
    void sendFileInfo();

    QString path;
    QString ip;
    QTcpSocket *socket;
    QFile *file;
    const qint64 BUFFER_SIZE = 327680;
    quint16 port = 4448;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    QJsonDocument doc;
    QJsonParseError docError;

signals:
    void blockUploaded(const qint64 progress) const;
    void fileUploaded() const;
    void connected();

public slots:
    void socketReady();
    void socketDisconnect();
};

#endif // UPLOADER_H
