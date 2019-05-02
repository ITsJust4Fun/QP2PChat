#include "StartWidget.h"
#include "ui_StartWidget.h"

StartWidget::StartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWidget)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton, SIGNAL(pressed()), this, SLOT(okPressed()));
}

void StartWidget::okPressed()
{
    ui->lineEditIP->setEnabled(false);
    ui->lineEditUser->setEnabled(false);
    ui->pushButton->setEnabled(false);
    emit dataReady(ui->lineEditUser->text(), ui->lineEditIP->text());
    close();
}

StartWidget::~StartWidget()
{
    delete ui;
}
