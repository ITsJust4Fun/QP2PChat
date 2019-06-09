#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QWidget>

namespace Ui {
class DownloadManager;
}

class DownloadManager : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadManager(QWidget *parent = nullptr);
    ~DownloadManager();

private:
    Ui::DownloadManager *ui;
};

#endif // DOWNLOADMANAGERDIALOG_H
