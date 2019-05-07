#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>
#include <QHostInfo>
#include <QMessageBox>

namespace Ui {
class StartWidget;
}

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(QWidget *parent = nullptr);
    ~StartWidget();

public slots:
    void okPressed();

signals:
    void dataReady(const QString &user, const QString &ip, const QString &mask);

private:
    QStringList ipList;
    Ui::StartWidget *ui;
};

#endif // STARTWIDGET_H
