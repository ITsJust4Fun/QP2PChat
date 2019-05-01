#include "Chat.h"
#include "ui_Chat.h"

Chat::Chat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    server = new Server();
    server->startServer(4444);

    socket = new QTcpSocket(this);

    ui->setupUi(this);
    connectAll();
}

void Chat::connectAll()
{
    QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
    QObject::connect(ui->buttonSend, SIGNAL(pressed()), this, SLOT(test()));
}

void Chat::test()
{
    //socket->connectToHost("127.0.0.1", 4444);
    socket->connectToHost("192.168.0.103", 4444);
}

void Chat::socketReady()
{

}

void Chat::socketDisconnect()
{
    socket->deleteLater();
}

void Chat::closeEvent (QCloseEvent *event)
{
    socket->disconnectFromHost();
    event->accept();
}

Chat::~Chat()
{
    delete ui;
    delete server;
}
