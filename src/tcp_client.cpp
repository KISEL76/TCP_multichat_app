#include "tcp_client.h"
#include "ui_tcp_client.h"

tcp_client::tcp_client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tcp_client)
{
    ui->setupUi(this);

    ui->pushButton_send->setEnabled(false);

    ui->plainTextEdit_chat->setReadOnly(true);
    setWindowTitle("Клиент");
    setWindowIcon(QIcon(":/icons/resources/chat_icon.png"));
}

tcp_client::~tcp_client()
{
    delete ui;
}

void tcp_client::on_pushButton_connect_clicked()//подключение к серверу
{
    if (!alreadyConnected)
    {
        if (ui->lineEdit_name->text().length() != 0) {
            nickname = ui->lineEdit_name->text();
        } else {
            QMessageBox::information(this, "Внимание", "Придумайте никнейм");
            return;
        }
        socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::readyRead, this, &tcp_client::slotReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &tcp_client::slotDisconnected);

        socket->connectToHost(ui->lineEdit_ip->text(), ui->spinBox_port->value());
        if (socket->waitForConnected(4000))
        {
            ui->pushButton_send->setEnabled(true);
            ui->lineEdit_name->setEnabled(false);
            ui->pushButton_nameColor->setEnabled(false);
            ui->lineEdit_ip->setEnabled(false);
            ui->spinBox_port->setEnabled(false);
            ui->pushButton_connect->setText("Откл");
            ui->plainTextEdit_chat->clear();


            QByteArray sendArr;
            sendArr.clear();
            DataStruct sendData;

            QDataStream sendStream(&sendArr, QIODevice::WriteOnly);

            sendData.name = nickname;

            sendStream << sendData;
            socket->write(sendArr);


            alreadyConnected = true;
            socks.push_back(socket);
            qDebug() << "клиент подключен";

        }
        else
        {
            QMessageBox::warning(this,"Внимание","Нет подключения");
            socket->deleteLater();
            return;
        }
    }
    else
    {
        slotDisconnected();
    }
}

void tcp_client::keyPressEvent(QKeyEvent *event) //обработка нажатия return на клавиатуре
{
    if ((event -> key() == Qt::Key_Return))
        on_pushButton_send_clicked();
}

void tcp_client::slotReadyRead()//принятие сообщений
{
    QByteArray recieveArr;
    DataStruct recieveData;
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);

    qDebug() << "прием клиент";

    if(in.status() == QDataStream::Ok)
    {
       in >> recieveData;
       if (recieveData.text.trimmed().length() != 0)
       {
            ui->plainTextEdit_chat->appendHtml("<i>" + recieveData.time + "</i>" + " | " + "<span style=\"color:" + recieveData.nameColor + "\">" + recieveData.name + "</span>" + "<span style=\"color:red;\">" + " : " + recieveData.text + "</span>");
       }
       else
       {
           qDebug() << recieveData.connectedClients[0];
           ui->users_list->clear();
           for (auto name: recieveData.connectedClients)
           {
               qDebug() << "заполнение";
               ui->users_list->addItem(name);
           }
       }
    }
    else
    {
       qDebug() << "error datastream";
       return;
    }
}

void tcp_client::sendToServer()//отправка сообщений на сервер
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
    socket->write(sendArr);

    ui->plainTextEdit_chat->appendHtml("<i>" + sendData.time + "</i>" + " | " + "<span style=\"color:" + sendData.nameColor + "\">" + sendData.name + "</span>" + "<span style=\"color:blue;\">" + " : " + sendData.text + "</span>");
}

void tcp_client::slotDisconnected()//отключение от сервера
{
    ui->users_list->clear();
    ui->lineEdit_name->setEnabled(true);
    ui->pushButton_nameColor->setEnabled(true);
    ui->pushButton_send->setEnabled(false);
    ui->lineEdit_ip->setEnabled(true);
    ui->spinBox_port->setEnabled(true);
    ui->pushButton_connect->setText("Подкл");
    ui->plainTextEdit_chat->clear();

    alreadyConnected = false;
    socket->deleteLater();
}


void tcp_client::on_pushButton_send_clicked()//нажатие кнопки отправки на сервер
{
    if (alreadyConnected)
    {
        if (!ui->lineEdit_msg->text().trimmed().isEmpty())
        {
            sendToServer();
            ui->lineEdit_msg->clear();
        }
    }
}


void tcp_client::on_pushButton_nameColor_clicked()//цвет ника
{
    color = QColorDialog::getColor(Qt::green,this);
}

