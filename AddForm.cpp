#include "AddForm.h"
#include "ui_AddForm.h"

AddForm::AddForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddForm)
{
    setWindowIcon(QIcon(":/icons/add.png"));
    ui->setupUi(this);
    ui->lineEdit->setPlaceholderText("Enter ip address of new user");
    QObject::connect(ui->pushButton, SIGNAL(pressed()), this, SLOT(okPressed()));
}

/*
 * При нажатии на кнопку в меню данные
 * отправляются на слот клиенту.
 * Окно добавление клиента закрывается.
*/
void AddForm::okPressed()
{
    emit dataReady(ui->lineEdit->text());
    ui->lineEdit->clear();
    close();
}

AddForm::~AddForm()
{
    delete ui;
}
