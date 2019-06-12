#include "downloadmanager.h"
#include "ui_downloadmanager.h"

DownloadManager::DownloadManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadManager)
{
    ui->setupUi(this);

    downloadModel = new DownloadModel(this);
    progressDelegate = new ProgressDelegate(this);
    ui->treeView->setModel(downloadModel);
    ui->treeView->setItemDelegate(progressDelegate);

    downloader = new Downloader();
    uploader = new Uploader();
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

void DownloadManager::startUploading(DownloadItem *item)
{
    uploader->setIp(item->getIp());
    uploader->setPath(item->getPath());
    uploader->connectToServer();
}

void DownloadManager::startDownloading(DownloadItem *item)
{

}

DownloadModel *DownloadManager::getModel()
{
    return downloadModel;
}
