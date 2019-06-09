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
