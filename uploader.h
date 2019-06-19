#ifndef UPLOADER_H
#define UPLOADER_H

#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTimer>

#include "Server.h"
#include "items/downloaditem.h"

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = nullptr);
    virtual ~Uploader();

    void setIp(const QString &ip);
    QString getIp() const;
    void uploadFile();
    void uploadNextBlock();
    void connectToServer();
    void setFiles(QList<DownloadItem *> files);
    void sendFileInfo(const QString &path, const QString &size);

private:
    QString ip;
    QTcpSocket *socket;
    QFile *file;
    const qint64 BUFFER_SIZE = 327680;
    quint16 port = 4448;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    QJsonDocument doc;
    QJsonParseError docError;
    QList<DownloadItem *> files;
    DownloadItem *currentFile;
    qint64 numberOfBlocks;
    qint64 uploadedBlocks;
    QTimer *timer;
    const int MAX_DELAY = 10000;

signals:
    void blockUploaded(DownloadItem *item, const int progress) const;
    void uploaded();

public slots:
    void socketReady();
    void socketDisconnect();
    void disconnectFromDownloader();
};

#endif // UPLOADER_H
