#ifndef CHAT_H
#define CHAT_H

#include <QMainWindow>
#include <QCloseEvent>

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

    QTcpSocket *socket;
    Server *server;

public slots:
    void socketReady();
    void socketDisconnect();
    void test();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
