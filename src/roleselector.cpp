#include "roleselector.h"
#include "ui_roleselector.h"

roleselector::roleselector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::roleselector)
{
    ui->setupUi(this);

    setFixedSize(580, 380);

    for (int i = 0; i < ui->comboBox_selector->count(); ++i)
    {
        ui->comboBox_selector->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }

    setWindowTitle("Выбор роли");
    setWindowIcon(QIcon(":/icons/resources/chat_icon.png"));
}

roleselector::~roleselector()
{
    delete ui;
}

void roleselector::on_pushButton_acceptMode_clicked()
{
    if (ui->comboBox_selector->currentIndex() == 0)
    {
        tcp_client *client = new tcp_client;
        client->show();
    }
    else if (ui->comboBox_selector->currentIndex() == 1)
    {
        tcp_server *server = new tcp_server;
        server->show();
    }

    close();
}
