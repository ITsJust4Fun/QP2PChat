#include "Server.h"

Server::Server()
{
    users = new QMap<QString, QMap<QString, QList<QString>>>();
}

void Server::startServer(quint16 port)
{
    this->listen(QHostAddress::Any, port);
}

void Server::incomingConnection(int socketDescriptor)
{
    sockets.append(new QTcpSocket(this));
    QTcpSocket *socket = sockets.last();
    socket->setSocketDescriptor(socketDescriptor);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));

    qDebug() << "connected " + socket->peerAddress().toString().mid(7);

    socket->write(QString("{" + head + ", " + "\"user\":"
                          + "\"" + localName + "\"}").toUtf8());
}

void Server::addNewUser(QJsonDocument &doc, QTcpSocket *socket)
{
    QString user = doc.object().value("user").toString();
    QString ip = socket->peerAddress().toString().mid(7);
    if (users->keys().contains(user)) {
        (*users)[user]["ip"][0] = ip;
        return;
    }
    QMap<QString, QList<QString>> info;
    QList<QString> list;
    QList<QString> ipList;
    ipList.append(ip);
    info.insert("ip", ipList);
    info.insert("messages", list);
    users->insert(user, info);
}

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
                + "\"" + "[" + localName + "]:" + msg + "\"}";
        socket->write(ans.toUtf8());
    }
}

void Server::addMsgToDatabase(const QString &user, const QString &msg)
{
    (*users)[user]["messages"].append(msg);
}

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

void Server::connectWithServerReq(const QString &ip)
{
    QString ans = "{" + head + ", " + "\"ip\":"
            + "\"" + ip + "\"}";
    sockets[0]->write(ans.toUtf8());
}

bool Server::isJsonValid(QJsonDocument &doc, QJsonParseError &docError)
{
    if ((docError.errorString().toInt() == QJsonParseError::NoError)
            && (doc.object().value("type").toString() == "p2p_connected")
            && (doc.object().value("status").toString() == "OK")) {
            return true;
    }
    return false;
}

void Server::socketDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    sockets.removeOne(socket);
    qDebug() << "disconnected " + socket->peerAddress().toString().mid(7);
    socket->deleteLater();
}

Server::~Server()
{
    delete users;
}
