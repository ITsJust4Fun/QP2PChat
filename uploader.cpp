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
        if (doc.object().value("downloader") == "next_file") {
            if (files.size() > 0) {
                currentFile = files.first();
                sendFileInfo(currentFile->getPathView(),
                             QString::number(QFile(currentFile->getPath()).size()));
            }
        } else if (doc.object().value("downloader") == "ready_download") {
            uploadFile();
            files.removeOne(currentFile);
        } else if (doc.object().value("downloader") == "next_block") {
            uploadNextBlock();
        }
    }
}

void Uploader::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "disconnected " + socket->peerAddress().toString();
    socket->deleteLater();
}

void Uploader::updateProgress(qint64 bytes)
{
    qDebug() << bytes;
}

void Uploader::sendFileInfo(const QString &path, const QString &size)
{
    QString ans = "{" + head + ", "
            + "\"path\":"
            + "\"" + path + "\", "
            + "\"size\":"
            + "\"" + size + "\"}";
    socket->write(ans.toUtf8());
}

void Uploader::connectToServer()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
    socket->connectToHost(ip, port);
}

void Uploader::setIp(const QString &ip)
{
    this->ip = ip;
}

QString Uploader::getIp() const
{
    return ip;
}

void Uploader::setFiles(QList<DownloadItem *> files)
{
    this->files = files;
}

void Uploader::uploadFile()
{
    file = new QFile(currentFile->getPath());
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "can't open file!!!";
        return;
    }
    qint64 fileSize = file->size();
    numberOfBlocks = fileSize / BUFFER_SIZE;
    numberOfBlocks = fileSize % BUFFER_SIZE != 0 ? numberOfBlocks + 1 : numberOfBlocks;
    uploadedBlocks = 0;
    uploadNextBlock();
}

void Uploader::uploadNextBlock()
{
    QByteArray arr = file->read(BUFFER_SIZE);
    socket->write(arr);
    socket->flush();
    uploadedBlocks++;
    emit blockUploaded(currentFile, static_cast<int>(uploadedBlocks * 100 / numberOfBlocks));
    if (numberOfBlocks == uploadedBlocks) {
        file->close();
        delete file;
    }
}
