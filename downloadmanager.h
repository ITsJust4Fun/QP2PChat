#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QWidget>

#include "models/downloadmodel.h"

namespace Ui {
class DownloadManager;
}

class DownloadManager : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadManager(QWidget *parent = nullptr);
    virtual ~DownloadManager();
    void appendUser(QString &user);

private:
    Ui::DownloadManager *ui;
    DownloadModel *downloadModel;
};

#endif // DOWNLOADMANAGERDIALOG_H
