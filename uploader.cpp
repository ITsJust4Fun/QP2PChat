#include "uploader.h"

Uploader::Uploader(QObject *parent) : QObject(parent)
{

}

Uploader::~Uploader()
{

}

void Uploader::socketReady()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = socket->readAll();
    doc = QJsonDocument::fromJson(data, &docError);

    if (Server::isJsonValid(doc, docError)) {
        if (doc.object().value("downloader") == "ready_download") {
            uploadFile();
        } else if (doc.object().value("downloader") == "connected") {
            emit connected();
        }
    }
}

void Uploader::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "disconnected " + socket->peerAddress().toString();
    socket->deleteLater();
}

void Uploader::sendFileInfo()
{

}

void Uploader::connectToServer()
{
    socket->connectToHost(ip, port);
}

void Uploader::setPath(const QString &path)
{
    this->path = path;
}

QString Uploader::getPath() const
{
    return path;
}

void Uploader::setIp(const QString &ip)
{
    this->ip = ip;
}

QString Uploader::getIp() const
{
    return ip;
}

void Uploader::uploadFile() const
{
    QFile *file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "can't open file!!!";
        return;
    }
    qint64 fileSize = file->size();
    qint64 numberOfBlocks = fileSize / BUFFER_SIZE;
    numberOfBlocks = fileSize % BUFFER_SIZE != 0 ? numberOfBlocks + 1 : numberOfBlocks;
    for (qint64 i = 0; i < numberOfBlocks; i++) {
        QByteArray arr;
        arr = file->read(BUFFER_SIZE);
        socket->write(arr);
        socket->flush();
        emit blockUploaded((i + 1) * 100 / numberOfBlocks);
    }
    emit fileUploaded();
    file->close();
    delete file;
}
