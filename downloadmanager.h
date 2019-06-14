#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QWidget>

#include "models/downloadmodel.h"
#include "models/downloadsortfilterproxymodel.h"
#include "progressdelegate.h"
#include "downloader.h"
#include "uploader.h"

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
    void appendDownload(const QString &user, const QString &path);
    void appendUpload(const QString &user, const QString &path);
    void startUploading(const QString &ip);
    void setUser(const QString &user);
    void setDownloadFolder(const QString &path);
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
    QList<DownloadItem *> uploadList;
    QMap<QString, QList<DownloadItem *> *> downloadList;

signals:
    void uploaderReady();

public slots:
};

#endif // DOWNLOADMANAGERDIALOG_H
