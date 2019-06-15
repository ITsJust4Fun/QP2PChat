#include "filespathsparser.h"

FilesPathsParser::FilesPathsParser(const QString &user, QStringList &paths,
                                   DownloadModel *model, QObject *parent) : QObject(parent)
{
    this->model = model;
    this->user = user;
    this->paths = new QStringList(paths);
    totalSize = 0;
}

FilesPathsParser::~FilesPathsParser()
{
    delete paths;
}

void FilesPathsParser::parseFileTree()
{
    DownloadItem *itemParent = model->appendTransfer(user);
    for (auto path : *paths) {
        QFile file(path);
        QFileInfo info(file);
        if (info.isDir()) {
            DownloadItem *item = model->appendDownload(itemParent, path);
            if (item)
                getAllFilesInFolder(path, item);
        } else {
            DownloadItem *item = model->appendDownload(itemParent, path);
            files.append(item);
            totalSize += info.size();
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

        DownloadItem* item = model->appendDownload(parent, entryPath);

        if (info.isDir()) {
            getAllFilesInFolder(entryPath, item);
        } else {
            files.append(item);
            totalSize += info.size();
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
