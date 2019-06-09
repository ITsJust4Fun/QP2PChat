#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>
#include <QHostInfo>
#include <QMessageBox>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

public slots:
    void okPressed();

signals:
    void dataReady(const QString &user, const QString &ip, const QString &mask);

private:
    QStringList ipList;
    Ui::Settings *ui;
};

#endif // STARTWIDGET_H
