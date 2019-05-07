#ifndef CHAT_H
#define CHAT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QHostInfo>
#include <QTimer>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QShortcut>
#include <QUdpSocket>

#include "Server.h"
#include "StartWidget.h"
#include "AddForm.h"

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
    void incomingMessage(const QString &user, const QString &msg);
    void addUnreadMessage(QListWidgetItem *item);
    void removeUnreadMessagesFlag(QListWidgetItem *item);
    QString getInitAnswer(const QString &user);
    bool isContainsConnection(const QString &ip);
    bool isListWidgetContains(const QString &user);
    void connectUdpSocket();
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
    AddForm *addForm;
    QShortcut *sendMsg;
    QUdpSocket *udpSocket;
    bool isDataSet;

public slots:
    void socketReady();
    void socketDisconnect();
    void sendMessage();
    void scan();
    void clearTimeSockets();
    void setData(const QString &user, const QString &ip);
    void getMessages(QListWidgetItem *item);
    void showAbout();
    void addUser(const QString &ip);
    void readUdp();

signals:
    void messageReceived(const QString &user, const QString &msg);

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
