#include "filespathsparser.h"

FilesPathsParser::FilesPathsParser(QStringList &paths, DownloadModel *model,
                                   QObject *parent) : QObject(parent)
{
    this->model = model;
    this->paths = new QStringList(paths);
    filesPaths = new QStringList();
}

FilesPathsParser::~FilesPathsParser()
{
    delete paths;
    delete filesPaths;
}

void FilesPathsParser::parseFileTree()
{
    model->appendUser("kek");
    for (auto path : *paths) {
        QFile file(path);
        QFileInfo info(file);
        if (info.isDir()) {
            DownloadItem *item = model->appendDownload("kek", path);
            if (item)
                getAllFilesInFolder(path, item);
        } else {
            filesPaths->append(path);
            model->appendDownload("kek", path);
        }
    }
    for (auto path : *filesPaths) {
        qDebug() << path;
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

        QList<QVariant> download;
        download << entryPath.mid(entryPath.lastIndexOf("/") + 1) << 0;
        DownloadItem *item = new DownloadItem(download, parent);
        item->setPath(entryPath);
        parent->appendChild(item);

        if (info.isDir()) {
            getAllFilesInFolder(entryPath, item);
        } else {
            filesPaths->append(entryPath);
        }
    }
}
