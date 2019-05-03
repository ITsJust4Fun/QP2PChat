#ifndef CHAT_H
#define CHAT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QHostInfo>
#include <QTimer>
#include <QMessageBox>

#include "Server.h"
#include "StartWidget.h"

namespace Ui {
class Chat;
}

class Chat : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    void connectAll();
    void connectSocket(QTcpSocket *socket);
    void connectToServer(const QString &ip);
    void closeEvent (QCloseEvent *event);
    bool isContainsConnection(const QString &ip);
    bool isListWidgetContains(const QString &user);
    ~Chat();

    QList<QTcpSocket *> sockets;
    QList<QTcpSocket *> timeSockets;
    QByteArray data;
    Server *server;
    QJsonDocument doc;
    QJsonParseError docError;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    quint16 port = 4444;
    QString addr;
    QString localName;
    QTimer *timer;
    StartWidget *startWidget;
    bool isDataSet;

public slots:
    void socketReady();
    void socketDisconnect();
    void sendMessage();
    void scan();
    void clearTimeSockets();
    void setData(const QString &user, const QString &ip);

signals:
    void messageReceived(const QString &user, const QString &msg);

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
