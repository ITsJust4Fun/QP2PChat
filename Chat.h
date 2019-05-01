#ifndef CHAT_H
#define CHAT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QHostInfo>
#include <QTimer>

#include "Server.h"

namespace Ui {
class Chat;
}

class Chat : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    void connectAll();
    void closeEvent (QCloseEvent *event);
    ~Chat();

    QList<QTcpSocket *> sockets;
    QList<QTcpSocket *> timeSockets;
    QByteArray data;
    Server *server;
    QJsonDocument doc;
    QJsonParseError docError;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    quint16 port = 4444;
    QString addr = "192.168.0.103";
    QString localName = "J4F";
    QString user = "OneWay";
    QTimer *timer;

public slots:
    void socketReady();
    void socketDisconnect();
    void sendMessage();
    void scan();
    void clearTimeSockets();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
