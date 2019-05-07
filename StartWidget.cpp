#include "StartWidget.h"
#include "ui_StartWidget.h"

StartWidget::StartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWidget)
{
    setWindowIcon(QIcon(":/icons/settings.png"));
    ui->setupUi(this);
    QList<QHostAddress> list = QHostInfo::fromName(QHostInfo::localHostName()).addresses();
    for (auto i : list) {
        QString ip = i.toString();
        if (!ip.contains(':')) {
            ipList.append(i.toString());
        }
    }
    ui->boxIp->addItems(ipList);
    ui->lineEditIp->setPlaceholderText("Fill if your ip is missing");
    QObject::connect(ui->pushButton, SIGNAL(pressed()), this, SLOT(okPressed()));
}

/*
 * При нажатии на кнопку в меню
 * элементы ввода блокируются, а данные
 * отправляются на слоты (клиенту и серверу).
 * Окно настроек закрывается
*/
void StartWidget::okPressed()
{
    QString ip = ui->lineEditIp->text() == "" ? ui->boxIp->currentText() : ui->lineEditIp->text();
    QString mask = ui->lineEditMask->text();
    if (ip.count(".") != mask.count(".")) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Mask or ip error format!");
        messageBox.setFixedSize(500,200);
        return;
    }
    ui->boxIp->setEnabled(false);
    ui->lineEditUser->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->lineEditIp->setEnabled(false);
    ui->lineEditMask->setEnabled(false);
    emit dataReady(ui->lineEditUser->text(), ip, mask);
    close();
}

StartWidget::~StartWidget()
{
    delete ui;
}
