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

    connect(uploader, SIGNAL(blockUploaded(DownloadItem *, const int)),
            downloadModel, SLOT(setProgress(DownloadItem *, const int)));
}

DownloadManager::~DownloadManager()
{
    delete ui;
}

void DownloadManager::appendUser(const QString &user)
{
    downloadModel->appendUser(user);
}

void DownloadManager::appendDownload(const QString &user, const QString &path)
{
    downloadModel->appendDownload(user, path);
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
    downloader->downloadFolder = path;
}

QString DownloadManager::getDownloadFolder()
{
    return downloader->downloadFolder;
}

void DownloadManager::setUploadFiles(QList<DownloadItem *> files)
{
    uploader->setFiles(files);
}
