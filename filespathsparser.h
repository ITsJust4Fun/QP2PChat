#ifndef FILESPATHSPARSER_H
#define FILESPATHSPARSER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "models/downloadmodel.h"

class FilesPathsParser : public QObject
{
    Q_OBJECT
public:
    explicit FilesPathsParser(const QString &user, QStringList &paths,
                              DownloadModel *model, int mode, QObject *parent = nullptr);
    QList<DownloadItem *> getFiles();
    qint64 getTotalSize();
    QString getUser();
    virtual ~FilesPathsParser();

private:
    void getAllFilesInFolder(QString &folderPath, DownloadItem *parent);

    QString user;
    QStringList *paths;
    QList<DownloadItem *> files;
    DownloadModel *model;
    qint64 totalSize;
    int mode;

signals:
    void treeIsReady();

public slots:
    void parseFileTree();
};

#endif // FILESPATHSPARSER_H
