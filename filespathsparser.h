#ifndef FILESPATHSPARSER_H
#define FILESPATHSPARSER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "models/downloadmodel.h"

class FilesPathsParser : public QObject
{
    Q_OBJECT
public:
    explicit FilesPathsParser(QStringList &paths, DownloadModel *model, QObject *parent = nullptr);
    virtual ~FilesPathsParser();

private:
    void getAllFilesInFolder(QString &folderPath, DownloadItem *parent);

    QStringList *paths;
    QStringList *filesPaths;
    DownloadModel *model;

signals:
    void treeIsReady();

public slots:
    void parseFileTree();
};

#endif // FILESPATHSPARSER_H
