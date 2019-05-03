#include "Chat.h"
#include "ui_Chat.h"

Chat::Chat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    setWindowIcon(QIcon(":/icons/chat_icon.png"));

    timer = new QTimer();
    timer->setInterval(500);
    sockets.append(new QTcpSocket(this));

    server = new Server();
    server->startServer(port);
    isDataSet = false;

    startWidget = new StartWidget();

    ui->setupUi(this);
    connectAll();
}

void Chat::connectAll()
{
    QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionScan, SIGNAL(triggered()), this, SLOT(scan()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    QObject::connect(ui->actionSettings, SIGNAL(triggered()), startWidget, SLOT(show()));
    connectSocket(sockets[0]);
    QObject::connect(ui->buttonSend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(clearTimeSockets()));
    QObject::connect(this, SIGNAL(messageReceived(const QString &, const QString &)), server,
                     SLOT(addMsgToDatabase(const QString &, const QString &)));
    QObject::connect(server, SIGNAL(dataReady(const QString &, const QString &)),
                     this, SLOT(setData(const QString &, const QString &)));
    QObject::connect(startWidget, SIGNAL(dataReady(const QString &, const QString &)),
                     server, SLOT(setData(const QString &, const QString &)));
    QObject::connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
                     this, SLOT(getMessages(QListWidgetItem *)));

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
    if (!isDataSet) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Please set username and ip in settings");
        messageBox.setFixedSize(500,200);
        return;
    }
    QString part = addr.left(addr.lastIndexOf('.') + 1);
    for (int i = 0; i < 256; i++) {
        QString ip = part + QString::number(i);
        if (!isContainsConnection(ip)) {
            connectToServer(ip);
        }
    }
    timer->start();
}

void Chat::showAbout()
{
    QMessageBox messageBox;
    messageBox.information(nullptr, "About",
                           "QChat\nSite: https://github.com/ITsJust4Fun/QP2PChat\nOpen settings, enter info and chatting!");
    messageBox.setFixedSize(500,200);
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
    if (!isDataSet) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Please set username and ip in settings!");
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!ui->listWidget->currentItem()) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Please select user!");
        messageBox.setFixedSize(500,200);
        return;
    }
    QString user = ui->listWidget->currentItem()->text();
    QString msg = ui->messageEdit->toPlainText();
    ui->messageEdit->clear();
    if (ui->messageArea->toPlainText() == "\n") {
        ui->messageArea->clear();
    }
    ui->messageArea->setText(ui->messageArea->toPlainText() + msg + "\n");
    QString ans = "{" + head + ", " + "\"user\":"
            + "\"" + user + "\", "
            + "\"message\":"
            + "\"" + msg + "\"}";
    sockets[0]->write(ans.toUtf8());
}

void Chat::getMessages(QListWidgetItem *item)
{
    removeUnreadMessagesFlag(item);
    QStringList list = server->getMessagesFrom(item->text());
    ui->messageArea->setText(list.join("\n") + "\n");
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
            incomingMessage(user, msg);
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
                if (!isListWidgetContains(user)) {
                    ui->listWidget->addItem(new QListWidgetItem(user));
                }
                sockets.append(socket);
                socket->write(getInitAnswer(user).toUtf8());
            }
        }
    }
}

QString Chat::getInitAnswer(const QString &user)
{
    QString answer = "{" + head + ", " + "\"user\":"
            + "\"" + localName + "\", " + "\"messages\": [";
    QStringList list = server->getMessagesFrom(user);
    if (list.isEmpty()) {
        answer += "]}";
        return answer;
    }
    answer += "\"";
    QString msgs = list.join("\", \"");
    answer += msgs + "\"]}";
    return answer;
}

void Chat::incomingMessage(const QString &user, const QString &msg)
{
    if (ui->listWidget->currentItem()) {
        if (ui->listWidget->currentItem()->text() == user) {
            ui->messageArea->setText(ui->messageArea->toPlainText() + msg + "\n");
            return;
        }
    }
    QListWidgetItem *userItem = ui->listWidget->findItems(user, Qt::MatchContains)[0];
    addUnreadMessage(userItem);
}

void Chat::addUnreadMessage(QListWidgetItem *item)
{
    QString user = item->text();
    if ((user.lastIndexOf('[') != -1)
            && (user.lastIndexOf(']') != -1)) {
        int left = user.lastIndexOf('[');
        int right = user.lastIndexOf(']');
        QString snum = user.mid(left + 1, right - left - 1);
        int num = snum.toInt();
        num++;
        item->setText(user.left(left) + '[' + QString::number(num) + ']');
    } else {
        item->setText(user + "[1]");
    }
}

void Chat::removeUnreadMessagesFlag(QListWidgetItem *item)
{
    QString user = item->text();
    if ((user.lastIndexOf('[') != -1)
            && (user.lastIndexOf(']') != -1)) {
        item->setText(user.left(user.lastIndexOf('[')));
    }
}

void Chat::setData(const QString &user, const QString &ip)
{
    addr = ip;
    localName = user;
    sockets[0]->connectToHost(addr, port);
    isDataSet = true;
}

bool Chat::isListWidgetContains(const QString &user)
{
    if (ui->listWidget->findItems(user, Qt::MatchContains).size() != 0) {
        return true;
    }
    return false;
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
    delete startWidget;
}
