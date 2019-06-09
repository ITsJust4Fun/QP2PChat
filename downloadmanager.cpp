#include "downloadmanager.h"
#include "ui_downloadmanager.h"

DownloadManager::DownloadManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadManager)
{
    ui->setupUi(this);

    downloadModel = new DownloadModel(this);
    ui->treeView->setModel(downloadModel);
    ui->treeView->setRootIsDecorated(false);
}

DownloadManager::~DownloadManager()
{
    delete ui;
}

void DownloadManager::appendUser(QString &user)
{
    downloadModel->appendUser(user);
}
