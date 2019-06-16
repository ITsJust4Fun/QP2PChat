#include "Server.h"

Server::Server()
{
    users = new QMap<QString, QMap<QString, QList<QString>>>();
}

/*
 * Запуск сервера
*/
void Server::startServer(quint16 port)
{
    this->listen(QHostAddress::Any, port);
}

/*
 * При входящем соединении.
 * 1)Создаётся и запоминается сокет
 * 2)Выводится дебаг сообщение об успешном соединении
 * 3)Отправляется ответ клиенту об успешном соединении
*/
void Server::incomingConnection(int socketDescriptor)
{
    if (localName == "") {
        return;
    }
    sockets.append(new QTcpSocket(this));
    QTcpSocket *socket = sockets.last();
    socket->setSocketDescriptor(socketDescriptor);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));

    qDebug() << "connected " + socket->peerAddress().toString().mid(7);

    socket->write(QString("{" + head + ", " + "\"user\":"
                          + "\"" + localName + "\"}").toUtf8());
}

/*
 * Добавление нового пользователя
 * в базу данных сообщений.
*/
void Server::addNewUser(QJsonDocument &doc, QTcpSocket *socket)
{
    QString user = doc.object().value("user").toString();
    QString ip = socket->peerAddress().toString().mid(7);
    if (users->keys().contains(user)) {
        (*users)[user]["ip"][0] = ip;
        return;
    }
    QMap<QString, QList<QString>> info;
    QJsonArray messages = doc.object().value("messages").toArray();
    QStringList list;
    for (auto message : messages) {
        list.append(message.toString());
    }
    //QList<QString> list = changeSenderInMessages(messages, user);
    QStringList ipList;
    ipList.append(ip);
    info.insert("ip", ipList);
    info.insert("messages", list);
    users->insert(user, info);
}

/*
 * Внесение данных о сообщениях
 * от удалённого сервера
*/
QList<QString> Server::changeSenderInMessages(QJsonArray &messages, const QString &user)
{
    QList<QString> list;
    for (auto i : messages) {
        QString message = i.toString();

        if (message.indexOf("]")) {
            message = message.mid(message.indexOf("]:") + 2);
        } else {
            message = "[" + user + "]: " + message;
        }
        list.append(message);
    }
    return list;
}

/*
 * Отправка исходящего сообщения клиенту
*/
void Server::sendMessage(QJsonDocument &doc)
{
    QString user = doc.object().value("user").toString();
    QString msg = doc.object().value("message").toString();
    (*users)[user]["messages"].append(msg);
    QTcpSocket *socket = findSocket(user);
    if (socket) {
        QString ans = "{" + head + ", " + "\"user\":"
                + "\"" + localName + "\", "
                + "\"message\":"
                + "\"" + msg + "\"}";
        socket->write(ans.toUtf8());
    }
}

/*
 * Добавление входящего сообщения в базу данных
*/
void Server::addMsgToDatabase(const QString &user, const QString &msg)
{
    (*users)[user]["messages"].append(msg);
}

/*
 * Поиск сокета по имени пользователя
*/
QTcpSocket * Server::findSocket(QString name)
{
    QTcpSocket * socket = nullptr;
    QString ip = (*users)[name]["ip"][0];
    for (auto i : sockets) {
        if (ip == i->peerAddress().toString().mid(7)) {
            socket = i;
        }
    }
    return socket;
}

/*
 * Установка имени пользователя из настроек приложения
*/
void Server::setData(const QString &user, const QString &ip, const QString &mask)
{
    localName = user;
    emit dataReady(user, ip, mask);
}

/*
 * Обработчик событий сокета.
 * В зависимости от содержания JSON сообщения
 * происходят те или иные действия.
*/
void Server::socketReady()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    data = socket->readAll();
    doc = QJsonDocument::fromJson(data, &docError);

    if (isJsonValid(doc, docError)) {
        if (doc.object().value("message") != QJsonValue::Undefined) {
            sendMessage(doc);
        } else {
            addNewUser(doc, socket);
            QString ip = socket->peerAddress().toString().mid(7);
            connectWithServerReq(ip);
        }
        return;
    }
    socket->disconnectFromHost();
}

/*
 * Запрос от сервера клиенту на соединение
 * с удалённым сервером.
 * Позваляет добавить в контакты пользователя, который
 * ищет собеседников по сети.
*/
void Server::connectWithServerReq(const QString &ip)
{
    QString ans = "{" + head + ", " + "\"ip\":"
            + "\"" + ip + "\"}";
    sockets[0]->write(ans.toUtf8());
}

/*
 * Проверка JSON сообщения на правильность
*/
bool Server::isJsonValid(QJsonDocument &doc, QJsonParseError &docError)
{
    if ((docError.errorString().toInt() == QJsonParseError::NoError)
            && (doc.object().value("type").toString() == "p2p_connected")
            && (doc.object().value("status").toString() == "OK")) {
            return true;
    }
    return false;
}

/*
 * Возвращяет список всех сообщений от пользователя
*/
QStringList Server::getMessagesFrom(const QString &user)
{
    if (!users->contains(user)) {
        QStringList list;
        return list;
    }
    QStringList list;
    list.append((*users)[user]["messages"]);
    return list;
}

/*
 * Действия при отключении от
 * сервера
*/
void Server::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    sockets.removeOne(socket);
    qDebug() << "disconnected " + socket->peerAddress().toString().mid(7);
    socket->deleteLater();
}

void Server::sendUploadRequest(const QString &user, QList<DownloadItem *> files, qint64 size)
{
    QTcpSocket *socket = findSocket(user);
    QString answer = "{" + head + ", " + "\"user\":"
            + "\"" + localName + "\", "
            + "\"downloader\":"
            + "\"" + "try_upload" + "\", "
            + "\"size\":"
            + "\"" + QString::number(size) + "\", "
            + "\"files\": [";
    QStringList paths;
    for (auto path : files) {
        paths.append(path->getPathView());
    }
    if (paths.isEmpty()) {
        answer += "]}";
    } else {
        answer += "\"";
        QString msgs = paths.join("\", \"");
        answer += msgs + "\"]}";
    }
    socket->write(answer.toUtf8());
}

void Server::acceptUploadRequest(const QString &user)
{
    QTcpSocket *socket = findSocket(user);
    QString ans = "{" + head + ", " + "\"user\":"
            + "\"" + user + "\", "
            + "\"downloader\":"
            + "\"" + "upload_accepted" + "\"}";
    socket->write(ans.toUtf8());
}

void Server::rejectUploadRequest(const QString &user)
{
    QTcpSocket *socket = findSocket(user);
    QString ans = "{" + head + ", " + "\"user\":"
            + "\"" + user + "\", "
            + "\"downloader\":"
            + "\"" + "upload_rejected" + "\"}";
    socket->write(ans.toUtf8());
}

Server::~Server()
{
    delete users;
}
