#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QWidget>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>

#include "models/downloadmodel.h"
#include "models/downloadsortfilterproxymodel.h"
#include "progressdelegate.h"
#include "downloader.h"
#include "uploader.h"
#include "filespathsparser.h"

namespace Ui {
class DownloadManager;
}

class DownloadManager : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadManager(QWidget *parent = nullptr);
    virtual ~DownloadManager();
    void appendUser(const QString &user);
    void startUploading(const QString &ip);
    void setUser(const QString &user);
    void setDownloadFolder(const QString &path);
    void setDownloadFiles(const QJsonArray &files);
    void setUploadFiles(QList<DownloadItem *> files);
    QString getDownloadFolder();
    DownloadModel *getModel();

private:
    Ui::DownloadManager *ui;
    DownloadModel *downloadModel;
    DownloadSortFilterProxyModel *downloadSortFilterProxyModel;
    ProgressDelegate *progressDelegate;
    Downloader *downloader;
    Uploader *uploader;
    QThread *treeUpdater;
    FilesPathsParser *parser;
    QString downloadFolder;

signals:
    void uploaderReady();
    void readyDownload(const QString &user);
    void downloadFinished();
    void uploadFinished();

public slots:
    void onTreeViewReady();
    void onDownloaded();
    void onUploaded();
    void openFile(const QModelIndex &index);
};

#endif // DOWNLOADMANAGERDIALOG_H
