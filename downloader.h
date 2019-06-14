#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

#include "Server.h"

class Downloader : public QTcpServer
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);
    virtual ~Downloader() override;

    void incomingConnection(qintptr socketDescriptor) override;
    void setPath(const QString &path);
    void setUser(const QString &user);
    QString getPath() const;
    QString getUser() const;
    bool isDowloading();
    void startDownloading();
    void allowConnection(bool allowConnect);
    void rejectConnection(QTcpSocket *socket);


    QString downloadFolder;

private:
    void finishDownload();

    QTcpSocket *socket;
    bool allowConnect;
    bool isDownloadFinished;
    QString path;
    QString user;
    QFile *file;
    qint64 numberOfBlocks;
    qint64 downloadedBlocks;
    qint64 partOfBlock;
    qint64 fileSize;
    quint16 port = 4448;
    const qint64 BUFFER_SIZE = 327680;
    QString extension = ".download";
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    QJsonDocument doc;
    QJsonParseError docError;

signals:
    void pathChanged(const QString &path);
    void blockUploaded(const qint64 progress) const;
    void downloadFinished();

public slots:
    void startServer();
    void socketReady();
    void socketDisconnect();
};

#endif // DOWNLOADER_H
