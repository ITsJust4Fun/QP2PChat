#include "downloader.h"

Downloader::Downloader(QObject *parent) :
            QTcpServer(parent)
{
    allowConnect = false;
    isDownloadFinished = true;
    path = "";
    user = "";
    socket = nullptr;
    timer = new QTimer(this);
    timer->setInterval(MAX_DELAY);
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
    connect(timer, SIGNAL(timeout()), this, SLOT(disconnectFromUploader()));

    qDebug() << "connected " + socket->peerAddress().toString().mid(7);
    sendMessage("next_file");
    allowConnect = false;
    timer->start();
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
    timer->start();
    QByteArray data = socket->readAll();
    doc = QJsonDocument::fromJson(data, &docError);
    if (isDownloadFinished) {
        if (Server::isJsonValid(doc, docError)) {
            if (doc.object().value("downloader") != QJsonValue::Undefined) {
                socket->disconnectFromHost();
                emit downloaded();
                return;
            }
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
        sendMessage("next_block");
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
    timer->stop();
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "disconnected " + socket->peerAddress().toString().mid(7);
    socket->deleteLater();
    if (!isDownloadFinished) {
        file->close();
        delete file;
        isDownloadFinished = true;
        for (auto item : files) {
            QFile(item->getPath()).remove();
        }
        files.clear();
        emit downloaded();
    }
    this->socket = nullptr;
}

void Downloader::rejectConnection(QTcpSocket *socket)
{
    QString answer = "{" + head + ", " + "\"downloader\":"
            + "\"" + "reject" + "\"}";
    socket->write(answer.toUtf8());
    socket->disconnectFromHost();
    socket->deleteLater();
}

void Downloader::finishDownload()
{
    file->close();
    isDownloadFinished = true;
    files.removeOne(currentFile);
    sendMessage("next_file");
    delete file;
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

void Downloader::disconnectFromUploader()
{
    socket->disconnectFromHost();
}
