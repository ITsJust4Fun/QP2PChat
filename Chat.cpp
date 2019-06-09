#include "Chat.h"
#include "ui_Chat.h"

Chat::Chat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    setWindowIcon(QIcon(":/icons/chat_icon.png"));

    sendMsg = new QShortcut(this);
    sendMsg->setKey(Qt::CTRL + Qt::Key_Return);

    timer = new QTimer();
    timer->setInterval(500);
    udpTimer = new QTimer();
    udpTimer->setInterval(1000);
    sockets.append(new QTcpSocket(this));

    server = new Server();
    server->startServer(port);
    isDataSet = false;

    settings = new Settings();
    addForm = new AddForm();

    udpSocketSender = new QUdpSocket(this);
    udpSocketReceiver = new QUdpSocket(this);
    connectUdpSocket();

    ui->setupUi(this);
    connectAll();
}

/*
 * Соединяет слоты с сигналами
*/
void Chat::connectAll()
{
    QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    QObject::connect(ui->actionScan, SIGNAL(triggered()), this, SLOT(scan()));

    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

    QObject::connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(show()));

    QObject::connect(ui->actionAddUser, SIGNAL(triggered()), addForm, SLOT(show()));

    connectSocket(sockets[0]);

    QObject::connect(ui->buttonSend, SIGNAL(pressed()), this, SLOT(sendMessage()));

    QObject::connect(sendMsg, SIGNAL(activated()), this, SLOT(sendMessage()));

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(clearTimeSockets()));

    QObject::connect(udpTimer, SIGNAL(timeout()), this, SLOT(addUdpUsers()));

    QObject::connect(this, SIGNAL(messageReceived(const QString &, const QString &)), server,
                     SLOT(addMsgToDatabase(const QString &, const QString &)));

    QObject::connect(server, SIGNAL(dataReady(const QString &, const QString &, const QString &)),
                     this, SLOT(setData(const QString &, const QString &, const QString &)));

    QObject::connect(settings, SIGNAL(dataReady(const QString &, const QString &, const QString &)),
                     server, SLOT(setData(const QString &, const QString &, const QString &)));

    QObject::connect(addForm, SIGNAL(dataReady(const QString &)),
                     this, SLOT(addUser(const QString &)));

    QObject::connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
                     this, SLOT(getMessages(QListWidgetItem *)));

}


/*
 * Соединяет сокет с обработчиками данных
*/
void Chat::connectSocket(QTcpSocket *socket)
{
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
}

/*
 * Соединение с сервером (временное, если сервер не ответит)
*/
void Chat::connectToServer(const QString &ip)
{
    timeSockets.append(new QTcpSocket(this));
    timeSockets.last()->connectToHost(ip, port);
    connectSocket(timeSockets.last());
}

/*
 * Настройка udp сокетов
*/
void Chat::connectUdpSocket()
{
    udpSocketReceiver->bind(QHostAddress::Any, port);
    udpSocketSender->open(QIODevice::WriteOnly);
    udpSocketReceiver->open(QIODevice::ReadOnly);
    QObject::connect(udpSocketReceiver, SIGNAL(readyRead()), this, SLOT(readUdp()));
}

/*
 * Добавление пользователя,
 * который откликнулся на udp запрос
 * в список или отклик на
 * широковещательный запрос
*/
void Chat::readUdp()
{
    QByteArray data;
    data.resize(static_cast<int>(udpSocketReceiver->pendingDatagramSize()));
    udpSocketReceiver->readDatagram(data.data(), data.size());
    doc = QJsonDocument::fromJson(data, &docError);

    if (Server::isJsonValid(doc, docError)) {
        if (doc.object().value("user") != QJsonValue::Undefined
                && doc.object().value("ip") != QJsonValue::Undefined) {
            QString user = doc.object().value("user").toString();
            QString ip = doc.object().value("ip").toString();
            if (doc.object().value("broadcast") != QJsonValue::Undefined) {
                QByteArray data = QString("{" + head + ", " + "\"user\":"
                                          + "\"" + localName + "\"" + ", " + "\"ip\":"
                                          + "\"" + addr + "\"}").toUtf8();
                udpSocketSender->writeDatagram(data, QHostAddress(ip), port);
            } else {
                if (user != localName && !isContainsConnection(ip)) {
                    udpAddrs.append(ip);
                    udpTimer->start();
                }
            }
        }
    }
}

/*
 * Попытка соединится с откликнувшимися
 * пользователями
*/
void Chat::addUdpUsers()
{
    for(auto i : udpAddrs) {
        addUser(i);
    }
    udpAddrs.clear();
    udpTimer->stop();
}

/*
 * Метод, который сканирует сеть,
 * отправляя запросы на соединение.
 * Если сервер ответит на запрос в нужном
 * формате, то соединение установлено.
*/
void Chat::scan()
{
    if (!isDataSet) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", noDataErr);
        messageBox.setFixedSize(500,200);
        return;
    }
    QByteArray data = QString("{" + head + ", " + "\"user\":"
                              + "\"" + localName + "\"" + ", " + "\"ip\":"
                              + "\"" + addr + "\"" + ", " + "\"broadcast\":"
                              + "\"" + "true" + "\"" + "}").toUtf8();
    udpSocketSender->writeDatagram(data, QHostAddress(broadcastIp), port);
}

/*
 * Вычисление широковещательного ip
*/
QString Chat::calcBroadcastIp(const QString &ip, const QString &mask)
{
    QStringList addr = ip.split(".");
    QStringList sMask = mask.split(".");
    QStringList netAddr;
    QStringList bIp;
    for (int i = 0; i < addr.size(); i++) {
        unsigned char byte = static_cast<unsigned char>(addr[i].toUInt())
                    & static_cast<unsigned char>(sMask[i].toUInt());
        netAddr << QString::number(byte);
    }
    for (int i = 0; i < netAddr.size(); i++) {
        unsigned char byte = static_cast<unsigned char>(netAddr[i].toUInt())
                    | ~static_cast<unsigned char>(sMask[i].toUInt());
        bIp << QString::number(byte);
    }
    return bIp.join(".");
}

/*
 * Показ окна About
*/
void Chat::showAbout()
{
    QMessageBox messageBox;
    messageBox.information(nullptr, "About",
                           "QChat\nSite: https://github.com/ITsJust4Fun/QP2PChat\nOpen settings, enter info and chatting!");
    messageBox.setFixedSize(500,200);
}

/*
 * Проверка ip. Если соединение уже установлено,
 * то true, если нет, то false.
*/
bool Chat::isContainsConnection(const QString &ip)
{
    for (auto i : sockets) {
        if (i->peerAddress().toString() == ip) {
            return true;
        }
    }
    return false;
}


/*
 * Отправление сообщения.
 * Оно отправится связанному серверу
 * для отправки на удалённый клиент.
*/
void Chat::sendMessage()
{
    if (!isDataSet) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", noDataErr);
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!ui->listWidget->currentItem()) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", selectUserErr);
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

/*
 * Получение от связанного сервера списка
 * сообщения с пользователем, при этом
 * флаг непрочитанных сообщений снимается.
*/
void Chat::getMessages(QListWidgetItem *item)
{
    removeUnreadMessagesFlag(item);
    QStringList list = server->getMessagesFrom(item->text());
    ui->messageArea->setText(list.join("\n") + "\n");
}

/*
 * Обработчик событий сокета.
 * В зависимости от содержания JSON сообщения
 * происходят те или иные действия.
*/
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

/*
 * Возврящяет JSON сообщение при соединении с удалённым сервером.
 * В этом ответе содержится список всех сообщений с конкретным
 * пользователем.
*/
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

/*
 * При входящем сообщении.
 * Либо устанавливается флаг(прибавляется к флагу) непрочитанных сообщений,
 * либо вывод сообщения в область для сообщений, если диалог с отправителем
 * открыт.
*/
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

/*
 * Прибавляет к флагу или устанавливает флаг
 * непрочитанных сообщений.
*/
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

/*
 * Убирает флаг непрочитанных сообщений
*/
void Chat::removeUnreadMessagesFlag(QListWidgetItem *item)
{
    QString user = item->text();
    if ((user.lastIndexOf('[') != -1)
            && (user.lastIndexOf(']') != -1)) {
        item->setText(user.left(user.lastIndexOf('[')));
    }
}

/*
 * Принамает данные пользователя
 * из настроек
*/
void Chat::setData(const QString &user, const QString &ip, const QString &mask)
{
    addr = ip;
    localName = user;
    this->mask = mask;
    broadcastIp = calcBroadcastIp(addr, mask);
    sockets[0]->connectToHost(addr, port);
    isDataSet = true;
}

/*
 * Пытается установить соединение
 * с пользователем
*/
void Chat::addUser(const QString &ip)
{
    if (!isDataSet) {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", noDataErr);
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!isContainsConnection(ip)) {
        connectToServer(ip);
        timer->start();
    }
}

/*
 * Проверка пользователя на наличие в
 * списке диалогов
*/
bool Chat::isListWidgetContains(const QString &user)
{
    if (ui->listWidget->findItems(user, Qt::MatchContains).size() != 0) {
        return true;
    }
    return false;
}

/*
 * Действия при отключении от
 * сервера
*/
void Chat::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    sockets.removeOne(socket);
    socket->deleteLater();
}

/*
 * Отчистка списка временных соедиенний.
 * Если соединение было установленно, то
 * оно не будет разорванно.
*/
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

/*
 * Разрыв соединений при закрытии
 * приложения
*/
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
    delete udpTimer;
    delete settings;
    delete addForm;
    delete sendMsg;
    delete udpSocketSender;
    delete udpSocketReceiver;
}
