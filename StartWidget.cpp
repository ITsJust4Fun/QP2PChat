#include "StartWidget.h"
#include "ui_StartWidget.h"

StartWidget::StartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWidget)
{
    ui->setupUi(this);
    QList<QHostAddress> list = QHostInfo::fromName(QHostInfo::localHostName()).addresses();
    for (auto i : list) {
        QString ip = i.toString();
        if (!ip.contains(':')) {
            ipList.append(i.toString());
        }
    }
    ui->boxIp->addItems(ipList);
    QObject::connect(ui->pushButton, SIGNAL(pressed()), this, SLOT(okPressed()));
}

void StartWidget::okPressed()
{
    ui->boxIp->setEnabled(false);
    ui->lineEditUser->setEnabled(false);
    ui->pushButton->setEnabled(false);
    emit dataReady(ui->lineEditUser->text(), ui->boxIp->currentText());
    close();
}

StartWidget::~StartWidget()
{
    delete ui;
}
