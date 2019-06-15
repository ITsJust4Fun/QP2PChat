#include "downloader.h"

Downloader::Downloader(QObject *parent) :
            QTcpServer(parent)
{
    allowConnect = false;
    isDownloadFinished = true;
    path = "";
    user = "";
    socket = nullptr;
}

Downloader::~Downloader()
{

}

void Downloader::incomingConnection(qintptr socketDescriptor)
{
    if (!allowConnect) {
        QTcpSocket *socket = new QTcpSocket(this);
        rejectConnection(socket);
        return;
    }

    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));

    qDebug() << "connected " + socket->peerAddress().toString().mid(7);
    sendMessage("next_file");
    allowConnect = false;
}

void Downloader::startDownloading()
{
    numberOfBlocks = fileSize / BUFFER_SIZE;
    numberOfBlocks = fileSize % BUFFER_SIZE == 0 ? numberOfBlocks : numberOfBlocks + 1;
    downloadedBlocks = 0;
    partOfBlock = 0;
    for (auto item : files) {
        if (item->getPathView() == path) {
            currentFile = item;
        }
    }
    QString filePath = currentFile->getPath();
    file = new QFile(filePath);
    if (!file->open(QIODevice::Append)) {
        return;
    }
    isDownloadFinished = false;
}

void Downloader::startServer()
{
    listen(QHostAddress::Any, port);
}

void Downloader::socketReady()
{
    QByteArray data = socket->readAll();
    doc = QJsonDocument::fromJson(data, &docError);
    if (isDownloadFinished) {
        if (Server::isJsonValid(doc, docError)) {
            path = doc.object().value("path").toString();
            fileSize = doc.object().value("size").toString().toLongLong();
            startDownloading();
            sendMessage("ready_download");
        }
        return;
    }
    file->write(data);
    partOfBlock += data.size();
    if (partOfBlock == BUFFER_SIZE) {
        downloadedBlocks++;
        partOfBlock = 0;
    }
    bool isLastBlockDownloaded = file->size() == fileSize;
    if (isLastBlockDownloaded) {
        downloadedBlocks++;
        finishDownload();
    }
    emit progressUpdated(currentFile, static_cast<int>(file->size() * 100 / fileSize));
}

void Downloader::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "disconnected " + socket->peerAddress().toString().mid(7);
    socket->deleteLater();
    if (!isDownloadFinished) {
        file->remove();
        isDownloadFinished = true;
    }
}

void Downloader::rejectConnection(QTcpSocket *socket)
{
    QString answer = "{" + head + ", " + "\"downloader\":"
            + "\"" + "reject" + "\"}";
    socket->write(answer.toUtf8());
    socket->disconnectFromHost();
    socket->deleteLater();
    socket = nullptr;
}

void Downloader::finishDownload()
{
    file->close();
    /*QFileInfo fileInfo(file->fileName());
    QString fileName = fileInfo.fileName();
    file->rename(fileName.left(fileName.lastIndexOf('.')));
    delete file;
    emit downloadFinished();*/
    isDownloadFinished = true;
    files.removeOne(currentFile);
    sendMessage("next_file");
}

void Downloader::setPath(const QString &path)
{
    this->path = path;
}

void Downloader::setUser(const QString &user)
{
    this->user = user;
    if (socket != nullptr) {
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = nullptr;
    }
}

QString Downloader::getPath() const
{
    return path;
}

QString Downloader::getUser() const
{
    return user;
}

bool Downloader::isDowloading()
{
    return !isDownloadFinished;
}

void Downloader::allowConnection(bool allowConnect)
{
    this->allowConnect = allowConnect;
}

void Downloader::sendMessage(const QString &message)
{
    QString answer = "{" + head + ", " + "\"downloader\":"
                     + "\"" + message + "\"}";
    socket->write(answer.toUtf8());
}

void Downloader::setDownloadFiles(QList<DownloadItem *> files)
{
    this->files = files;
}
