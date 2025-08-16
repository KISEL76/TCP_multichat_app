#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTime>
#include <QTimer>
#include <QColorDialog>
#include <QListWidget>
#include <QMessageBox>

namespace Ui {
class tcp_server;
}

class tcp_server : public QMainWindow
{
    Q_OBJECT

public:
    explicit tcp_server(QWidget *parent = nullptr);
    ~tcp_server();

    QTcpSocket *socket;
    QTcpServer *server;

    struct DataStruct{
        QString text;
        QString name;
        QString nameColor;
        QString time;
        QVector <QString> connectedClients;

        friend QDataStream & operator<<(QDataStream & in, DataStruct const & x)
        {
            return in << x.text << x.name << x.nameColor << x.time << x.connectedClients;
        }

        friend QDataStream & operator>>(QDataStream & out, DataStruct & x)
        {
            return out >> x.text >> x.name >> x.nameColor >> x.time;
        }
    };

    QColor color = "#2ba71e";
    QString nickname = "Сервер";

private:
    Ui::tcp_server *ui;

    QVector <QTcpSocket*> sockets;
    QByteArray Data;
    bool alreadyHosting = false;
    void SendToClient();
    int spamCounter = 0;
    bool socketConnected = false;
    QTime lastMessageTime;
    QTimer *timer = new QTimer(this);
    QVector <QString> bannedIp;
    QVector <QString> connectedUsers;
    QVector <QString> userNames;


    int blockSize;

public slots:
    void incomingConnections();
    void slotReadyRead();
    void slotDisconnect();

private slots:
    void on_pushButton_send_clicked();
    void on_pushButton_connect_clicked();
    void onTimerTimeout();
    void keyPressEvent(QKeyEvent *event);
    void on_listWidget_users_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_bannedUsers_itemDoubleClicked(QListWidgetItem *item);
};

#endif // TCP_SERVER_H
