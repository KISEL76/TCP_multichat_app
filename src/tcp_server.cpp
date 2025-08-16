#include "tcp_server.h"
#include "ui_tcp_server.h"

tcp_server::tcp_server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tcp_server)
{
    ui->setupUi(this);

    ui->pushButton_send->setEnabled(false);

    ui->plainTextEdit_chat->setReadOnly(true);

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    timer->start(1000);

    setWindowTitle("Сервер");
    setWindowIcon(QIcon(":/icons/resources/chat_icon.png"));
}

tcp_server::~tcp_server()
{
    delete ui;
}

void tcp_server::incomingConnections() //новое подключение
{
    while(server->hasPendingConnections())
    {
        socket = server->nextPendingConnection();

        QString users = socket->peerAddress().toString();
        QString us = users.mid(7);

        if (bannedIp.contains(us)) //проверка подключения через бан-лист
        {
            socket->disconnectFromHost();
            qDebug() << "бан";
            return;
        }
        connectedUsers.push_back(us);


        ui->listWidget_users->addItem(us);
        ui->pushButton_send->setEnabled(true);

        sockets.push_back(socket);
        connect(socket, &QTcpSocket::readyRead, this, &tcp_server::slotReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &tcp_server::slotDisconnect);

        socketConnected = true;
    }
}

void tcp_server::slotReadyRead() //Прием сообщений
{
    socket = (QTcpSocket*)sender();

    QString users = socket->peerAddress().toString();
    QString us = users.mid(7);

    if (bannedIp.contains(us)) //проверка подключения через бан-лист
    {
        socket->disconnectFromHost();
        qDebug() << "бан";
        return;
    }

    QDataStream in(socket);
    DataStruct recieveData;

    QByteArray recieveArr;
    recieveArr.clear();
    QDataStream recieveStream(&recieveArr, QIODevice::WriteOnly);


    if(in.status() == QDataStream::Ok)
    {
        in.startTransaction();
        in >> recieveData;

        QByteArray sendArr;
        sendArr.clear();
        DataStruct sendData = recieveData;

        if (recieveData.text.trimmed().length() == 0)
        {
            userNames.push_back(recieveData.name);
        }


        if(!in.commitTransaction())
        {
            qDebug() << "сообщение не дошло";
            return;
        }

        if (recieveData.text.size() > 4096)
        {
            QString users = socket->peerAddress().toString();
            QString us = users.mid(7);

            ui->listWidget_bannedUsers->addItem(us);
            bannedIp.push_back(us);

            socket->disconnectFromHost();
            return;

        }
        DataStruct toAll;

        toAll.name = recieveData.name;
        toAll.nameColor = recieveData.nameColor;
        toAll.time = recieveData.time;
        toAll.text = recieveData.text;

        recieveStream << toAll;
        if (recieveData.text.trimmed().length() != 0)
        {
            ui->plainTextEdit_chat->appendHtml("<i>" + recieveData.time + "</i>" + " | " + "<span style=\"color:" + recieveData.nameColor + "\">" + recieveData.name + "</span>" + "<span style=\"color:red;\">" + " : " + recieveData.text + "</span>");
        }

        for (auto user: sockets)
        {
            if (user != socket or recieveData.text.trimmed().length() == 0)
            {
                qDebug() << recieveData.text.trimmed().length();
                QDataStream sendStream(&sendArr, QIODevice::WriteOnly);

                sendData.connectedClients = userNames;
                qDebug() << sendData.connectedClients[0];
                sendStream << sendData;
                user->write(sendArr);

                qDebug() << "отправка";
            }
        }

        spamCounter += 1;
    }
    else
    {
        qDebug() << "error datastream";
        return;
    }
}

void tcp_server::slotDisconnect() //Слот отключения
{
    socket = (QTcpSocket*)sender();

    QString users = socket->peerAddress().toString();
    QString us = users.mid(7);

    ui->listWidget_users->takeItem(connectedUsers.indexOf(us));
    sockets.remove(connectedUsers.indexOf(us));

    userNames.removeAt(connectedUsers.indexOf(us));
    connectedUsers.remove(connectedUsers.indexOf(us));


    socket->disconnectFromHost();
    socket->deleteLater();

    socketConnected = false;

    ui->pushButton_send->setEnabled(false);

    QByteArray sendArr;
    sendArr.clear();
    DataStruct sendData;

    QDataStream sendStream(&sendArr, QIODevice::WriteOnly);

    sendData.connectedClients = userNames;

    sendStream << sendData;

    for (auto user: sockets)
    {
        user->write(sendArr);
    }
}

void tcp_server::SendToClient()//отправка сообщений клиенту
{
    QByteArray sendArr;
    sendArr.clear();
    DataStruct sendData;

    QDataStream sendStream(&sendArr, QIODevice::WriteOnly);

    sendData.text = ui->lineEdit_msg->text();
    sendData.name = nickname;
    sendData.nameColor = color.name();
    QDateTime cDate = QDateTime::currentDateTime();
    sendData.time = cDate.toString("hh:mm:ss");

    sendStream << sendData;
    for (auto user: sockets)
    {
        user->write(sendArr);
    }

    ui->plainTextEdit_chat->appendHtml("<i>" + sendData.time + "</i>" + " | " + "<span style=\"color:" + sendData.nameColor + "\">" + sendData.name + "</span>" + "<span style=\"color:blue;\">" + " : " + sendData.text + "</span>");
}

void tcp_server::on_pushButton_send_clicked() //Кнопка отправки сообщения
{
    if (socketConnected)
    {
        if (!ui->lineEdit_msg->text().trimmed().isEmpty())
        {
            SendToClient();
            ui->lineEdit_msg->clear();
        }
    }
}


void tcp_server::on_pushButton_connect_clicked()//кнопка включения
{
    if (!alreadyHosting)
    {
        if (ui->lineEdit_name->text().length() != 0) {
            nickname = ui->lineEdit_name->text();
        } else {
            QMessageBox::information(this, "Внимание", "Придумайте никнейм");
            return;
        }
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &tcp_server::incomingConnections);
        if (server->listen(QHostAddress::Any, ui->spinBox_port->value())) {
            ui->lineEdit_name->setEnabled(false);
            ui->pushButton_nameColor->setEnabled(false);
            ui->pushButton_connect->setText("Откл");
            ui->lineEdit_ip->setEnabled(false);
            ui->spinBox_port->setEnabled(false);
            ui->plainTextEdit_chat->clear();
            alreadyHosting = true;
        } else {
            QMessageBox::information(this, "Внимание", "Данный порт уже используется!");
            delete server;
            server = nullptr;
        }
    }
    else {
        if (server) {
            server->close();
            delete server;
            server = nullptr;
        }
        if (ui->lineEdit_name->text().length() != 0) {
            nickname = ui->lineEdit_name->text();
        } else {
            QMessageBox::information(this, "Внимание", "Придумайте никнейм");
            return;
        }
        ui->pushButton_send->setEnabled(false);
        ui->lineEdit_name->setEnabled(true);
        ui->pushButton_nameColor->setEnabled(true);
        ui->lineEdit_ip->setEnabled(true);
        ui->spinBox_port->setEnabled(true);
        ui->pushButton_connect->setText("Вкл");
        alreadyHosting = false;
    }
}

void tcp_server::keyPressEvent(QKeyEvent *event) //обработка нажатия return на клавиатуре
{
    if ((event -> key() == Qt::Key_Return))
        on_pushButton_send_clicked();
}

void tcp_server::onTimerTimeout()//прошла 1 секунда
{
    if (socketConnected)
    {
        if (spamCounter > 3)
        {
            QString banned = socket->peerAddress().toString();
            QString ban = banned.mid(7);

            bannedIp.push_back(ban);
            ui->listWidget_bannedUsers->addItem(ban);

            socket->disconnectFromHost();
        }
    }
    spamCounter = 0;
}

void tcp_server::on_listWidget_users_itemDoubleClicked(QListWidgetItem *item)//двойное нажатие на список подключённых
{

    ui->listWidget_bannedUsers->addItem(item->text());
    bannedIp.push_back((item->text()));

    for (QTcpSocket *client : sockets) {

        QString address = client ->peerAddress().toString();
        QString addr = address.mid(7);
        qDebug() << addr;
        qDebug() << item->text();
        if (addr == item->text())
        {
            client->disconnectFromHost();
        }
    }
}


void tcp_server::on_listWidget_bannedUsers_itemDoubleClicked(QListWidgetItem *item)//двойное нажатие на бан-список
{
    ui->listWidget_bannedUsers->takeItem(bannedIp.indexOf(item->text()));
    bannedIp.remove(bannedIp.indexOf(item->text()));
}

