#include "filespathsparser.h"

FilesPathsParser::FilesPathsParser(const QString &user, QStringList &paths,
                                   DownloadModel *model, int mode, QObject *parent) : QObject(parent)
{
    this->model = model;
    this->user = user;
    this->paths = new QStringList(paths);
    this->mode = mode;
    totalSize = 0;
    whiteList = nullptr;
}

FilesPathsParser::~FilesPathsParser()
{
    if (whiteList) {
        delete whiteList;
    }
    delete paths;
}

void FilesPathsParser::parseFileTree()
{
    DownloadItem *itemParent = model->appendTransfer(user, mode);
    for (auto path : *paths) {
        QFile file(path);
        QFileInfo info(file);
        if (info.isDir()) {
            DownloadItem *item = model->appendDownload(itemParent, path, mode);
            if (item)
                getAllFilesInFolder(path, item);
        } else {
            if (!whiteList) {
                DownloadItem *item = model->appendDownload(itemParent, path, mode);
                files.append(item);
                totalSize += info.size();
            } else if (whiteList->contains(path)) {
                DownloadItem *item = model->appendDownload(itemParent, path, mode);
                files.append(item);
                totalSize += info.size();
            }
        }
    }
    emit treeIsReady();
}

void FilesPathsParser::getAllFilesInFolder(QString &folderPath, DownloadItem *parent)
{
    QDir dir(folderPath);
    QStringList entryList = dir.entryList();
    for (auto entry : entryList) {
        if (entry == "." || entry == "..") {
            continue;
        }
        QString entryPath = folderPath + "/" + entry;
        QFile file(entryPath);
        QFileInfo info(file);

        if (info.isDir()) {
            DownloadItem* item = model->appendDownload(parent, entryPath, mode);
            getAllFilesInFolder(entryPath, item);
        } else {
            if (!whiteList) {
                DownloadItem* item = model->appendDownload(parent, entryPath, mode);
                files.append(item);
                totalSize += info.size();
            } else if (whiteList->contains(entryPath)) {
                DownloadItem* item = model->appendDownload(parent, entryPath, mode);
                files.append(item);
                totalSize += info.size();
            }
        }
    }
}

QList<DownloadItem *> FilesPathsParser::getFiles()
{
    return files;
}

qint64 FilesPathsParser::getTotalSize()
{
    return totalSize;
}

QString FilesPathsParser::getUser()
{
    return user;
}

void FilesPathsParser::setWhiteList(QStringList &list)
{
    if (whiteList) {
        delete whiteList;
    }
    whiteList = new QStringList(list);
}
