#include "downloadmanager.h"
#include "ui_downloadmanager.h"

DownloadManager::DownloadManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadManager)
{
    ui->setupUi(this);

    downloadModel = new DownloadModel(this);
    downloadSortFilterProxyModel = new DownloadSortFilterProxyModel(this);
    progressDelegate = new ProgressDelegate(this);
    downloadSortFilterProxyModel->setSourceModel(downloadModel);
    ui->treeView->setModel(downloadSortFilterProxyModel);
    ui->treeView->setItemDelegate(progressDelegate);

    downloader = new Downloader();
    uploader = new Uploader();

    downloader->startServer();
    ui->treeView->setSortingEnabled(true);
    ui->treeView->sortByColumn(DownloadItem::NameColumn, Qt::AscendingOrder);

    treeUpdater = new QThread(this);

    connect(uploader, SIGNAL(blockUploaded(DownloadItem *, const int)),
            downloadModel, SLOT(setProgress(DownloadItem *, const int)));
    connect(downloader, SIGNAL(progressUpdated(DownloadItem *, const int)),
            downloadModel, SLOT(setProgress(DownloadItem *, const int)));
    connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(openFile(const QModelIndex &)));
    connect(uploader, SIGNAL(uploaded()), this, SLOT(onUploaded()));
    connect(downloader, SIGNAL(downloaded()), this, SLOT(onDownloaded()));
}

DownloadManager::~DownloadManager()
{
    delete ui;
}

void DownloadManager::appendUser(const QString &user)
{
    downloadModel->appendUser(user);
}

void DownloadManager::startUploading(const QString &ip)
{
    uploader->setIp(ip);
    uploader->connectToServer();
}

void DownloadManager::setUser(const QString &user)
{
    downloader->setUser(user);
    downloader->allowConnection(true);
}

DownloadModel *DownloadManager::getModel()
{
    return downloadModel;
}

void DownloadManager::setDownloadFolder(const QString &path)
{
    downloadFolder = path;
}

QString DownloadManager::getDownloadFolder()
{
    return downloadFolder;
}

void DownloadManager::setUploadFiles(QList<DownloadItem *> files)
{
    uploader->setFiles(files);
}

void DownloadManager::setDownloadFiles(const QJsonArray &files)
{
    QStringList paths;
    for (auto i : files) {
        QString path = i.toString();
        QString filePath = downloadFolder + path;
        QString folderPath = filePath.left(filePath.lastIndexOf('/'));
        QFile *file = new QFile(filePath);
        int j = 1;
        QDir dir(folderPath);
        dir.mkpath(folderPath);
        while (!file->open(QIODevice::Append | QIODevice::NewOnly)) {
            delete file;
            int dotIndex = path.lastIndexOf('.');
            if (path[dotIndex - 1] == ')') {
                int leftIndex = path.lastIndexOf('(');
                int rightIndex = path.lastIndexOf(')');
                path = path.left(leftIndex + 1) + QString::number(j)
                                                + path.mid(rightIndex);
            } else {
                path = path.left(dotIndex) + "(" + QString::number(j)
                                           + ")" + path.mid(dotIndex);
            }
            file = new QFile(downloadFolder + path);
            j++;
        }
        paths.append(downloadFolder + path);
        file->close();
        delete file;
    }
    QDir dir(downloadFolder);
    QStringList entryList = dir.entryList();
    QStringList listForParser;
    for (auto entry : entryList) {
        for (auto path : paths) {
            QString entryPath = downloadFolder + "/" + entry;
            if (path.contains(entryPath)) {
                listForParser.append(entryPath);
                break;
            }
        }
    }
    parser = new FilesPathsParser(downloader->getUser(), listForParser,
                                  downloadModel, DownloadItem::DownloadMode);
    parser->setWhiteList(paths);
    parser->moveToThread(treeUpdater);
    connect(treeUpdater, SIGNAL(started()), parser, SLOT(parseFileTree()));
    connect(parser, SIGNAL(treeIsReady()), treeUpdater, SLOT(quit()));
    connect(parser, SIGNAL(treeIsReady()), this, SLOT(onTreeViewReady()));
    treeUpdater->start();
}

void DownloadManager::onTreeViewReady()
{
    downloader->setDownloadFiles(parser->getFiles());
    delete parser;
    emit readyDownload(downloader->getUser());
}

void DownloadManager::onDownloaded()
{
    emit downloadFinished();
}

void DownloadManager::onUploaded()
{
    emit uploadFinished();
}

void DownloadManager::openFile(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    QModelIndex sourceIndex = downloadSortFilterProxyModel->mapToSource(index);

    DownloadItem *item = static_cast<DownloadItem *>(sourceIndex.internalPointer());

    QStringList data = item->data(DownloadItem::ProgressColumn).toString().split(" ");
    int progressIndex = data.size() > 1 ? 1 : 0;
    int value = data[progressIndex].toInt();

    if (value != 100) {
        return;
    }

    QString path = "file://" + item->getPath();
    if (!path.isEmpty()) {
        QUrl url = QUrl::fromLocalFile(path);
        QDesktopServices::openUrl(url);
    }
}
