#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <QColorDialog>
#include <QMessageBox>

namespace Ui {
class tcp_client;
}

class tcp_client : public QMainWindow
{
    Q_OBJECT

public:
    explicit tcp_client(QWidget *parent = nullptr);
    ~tcp_client();

     QColor color = "#2ba71e";
     QString nickname = "безымянный";

     struct DataStruct{
         QString text;
         QString name;
         QString nameColor;
         QString time;
         QVector <QString> connectedClients;


         friend QDataStream & operator<<(QDataStream & in, DataStruct const & x)
         {
             return in << x.text << x.name << x.nameColor << x.time;
         }

         friend QDataStream & operator>>(QDataStream & out, DataStruct & x)
         {
             return out >> x.text >> x.name >> x.nameColor >> x.time >> x.connectedClients;
         }
     };

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_nameColor_clicked();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::tcp_client *ui;

    QTcpSocket *socket;
    QByteArray Data;
    bool alreadyConnected = false;
    QVector <QTcpSocket*> socks;


public slots:
    void slotReadyRead();
    void sendToServer();
    void slotDisconnected();

};

#endif // TCP_CLIENT_H
