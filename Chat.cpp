#include "Chat.h"
#include "ui_Chat.h"

Chat::Chat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    timer = new QTimer();
    timer->setInterval(500);
    sockets.append(new QTcpSocket(this));

    server = new Server();
    server->startServer(port);
    sockets[0]->connectToHost(addr, port);

    ui->setupUi(this);
    connectAll();
}

void Chat::connectAll()
{
    QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionScan, SIGNAL(triggered()), this, SLOT(scan()));
    connectSocket(sockets[0]);
    QObject::connect(ui->buttonSend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(clearTimeSockets()));
    QObject::connect(this, SIGNAL(messageReceived(const QString &, const QString &)), server,
                     SLOT(addMsgToDatabase(const QString &, const QString &)));
}

void Chat::connectSocket(QTcpSocket *socket)
{
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
}

void Chat::connectToServer(const QString &ip)
{
    timeSockets.append(new QTcpSocket(this));
    timeSockets.last()->connectToHost(ip, port);
    connectSocket(timeSockets.last());
}

void Chat::scan()
{
    QString part = "192.168.0.";
    for (int i = 0; i < 256; i++) {
        QString ip = part + QString::number(i);
        if (!isContainsConnection(ip)) {
            connectToServer(ip);
        }
    }
    timer->start();
}

bool Chat::isContainsConnection(const QString &ip)
{
    for (auto i : sockets) {
        if (i->peerAddress().toString() == ip) {
            return true;
        }
    }
    return false;
}

void Chat::sendMessage()
{
    QString msg = ui->messageEdit->toPlainText();
    ui->messageEdit->clear();
    ui->messageArea->setText(ui->messageArea->toPlainText() + msg + "\n");
    QString ans = "{" + head + ", " + "\"user\":"
            + "\"" + user + "\", "
            + "\"message\":"
            + "\"" + msg + "\"}";
    sockets[0]->write(ans.toUtf8());
}

void Chat::socketReady()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    data = socket->readAll();
    doc = QJsonDocument::fromJson(data, &docError);

    if (Server::isJsonValid(doc, docError)) {
        if (doc.object().value("message") != QJsonValue::Undefined) {
            QString msg = doc.object().value("message").toString();
            QString user = doc.object().value("user").toString();
            ui->messageArea->setText(ui->messageArea->toPlainText() + msg + "\n");
            emit messageReceived(user, msg);
        } else if (doc.object().value("ip") != QJsonValue::Undefined) {
            QString ip = doc.object().value("ip").toString();
            if (!isContainsConnection("ip") && !timer->isActive()) {
                connectToServer(ip);
                timer->start();
            }
        } else {
            if (!sockets.contains(socket)) {
                QString user = doc.object().value("user").toString();
                qDebug() << "added " + user;
                sockets.append(socket);
                socket->write(QString("{" + head + ", " + "\"user\":"
                        + "\"" + localName + "\"}").toUtf8());
            }
        }
        /*if (doc.object().size() == 3) {
            QString name = doc.object().value("user").toString();
            QString ans = "{" + head + ", " + "\"user\":"
                    + "\"" + localName + "\"}";
            socket->write(ans.toUtf8());
            ans = "{" + head + ", " + "\"user\":"
                    + "\"" + name + "\"}";
            sockets[0]->write(ans.toUtf8());
            ui->listWidget->addItem(new QListWidgetItem(name));
        } else if () {

        } else {

        }*/
    }
}

void Chat::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    sockets.removeOne(socket);
    socket->deleteLater();
}

void Chat::clearTimeSockets()
{
    for (auto i : timeSockets) {
        if (!sockets.contains(i)) {
            i->disconnectFromHost();
            i->deleteLater();
        }
    }
    timeSockets.clear();
    timer->stop();
}

void Chat::closeEvent (QCloseEvent *event)
{
    for (auto i : sockets) {
        i->disconnectFromHost();
    }
    event->accept();
}

Chat::~Chat()
{
    delete ui;
    delete server;
    delete timer;
}
