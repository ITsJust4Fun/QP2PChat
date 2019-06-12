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
#include <QScrollBar>
#include <QMimeData>
#include <QThread>

#include "Server.h"
#include "Settings.h"
#include "AddForm.h"
#include "downloadmanager.h"
#include "filespathsparser.h"

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
    void closeEvent (QCloseEvent *event) override;
    void incomingMessage(const QString &user, const QString &msg);
    void addUnreadMessage(QListWidgetItem *item);
    void removeUnreadMessagesFlag(QListWidgetItem *item);
    static QString calcBroadcastIp(const QString &ip, const QString &mask);
    QString getInitAnswer(const QString &user);
    bool isContainsConnection(const QString &ip);
    bool isListWidgetContains(const QString &user);
    void connectUdpSocket();
    void scrollToBottom();
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    virtual ~Chat() override;

    QList<QTcpSocket *> sockets;
    QList<QTcpSocket *> timeSockets;
    QList<QString> udpAddrs;
    Server *server;
    QJsonDocument doc;
    QJsonParseError docError;
    QString head = "\"type\":\"p2p_connected\", \"status\":\"OK\"";
    quint16 port = 4444;
    QString addr;
    QString mask;
    QString broadcastIp;
    QString localName;
    QTimer *timer;
    QTimer *udpTimer;
    Settings *settings;
    AddForm *addForm;
    DownloadManager *downloadManager;
    QShortcut *sendMsg;
    QUdpSocket *udpSocketSender;
    QUdpSocket *udpSocketReceiver;
    QString noDataErr = "Please set username, ip and mask in settings";
    QString selectUserErr = "Please select user!";
    bool isDataSet;
    QThread *treeFillThread;
    FilesPathsParser *parser;


public slots:
    void socketReady();
    void socketDisconnect();
    void sendMessage();
    void scan();
    void clearTimeSockets();
    void setData(const QString &user, const QString &ip, const QString &mask);
    void getMessages(QListWidgetItem *item);
    void showAbout();
    void addUser(const QString &ip);
    void readUdp();
    void addUdpUsers();
    void deleteParser();

signals:
    void messageReceived(const QString &user, const QString &msg);

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
