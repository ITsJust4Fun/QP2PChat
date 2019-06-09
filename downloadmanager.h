#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QWidget>

#include "models/downloadmodel.h"
#include "progressdelegate.h"

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

private:
    Ui::DownloadManager *ui;
    DownloadModel *downloadModel;
    ProgressDelegate *progressDelegate;
};

#endif // DOWNLOADMANAGERDIALOG_H
