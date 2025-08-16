#ifndef ROLESELECTOR_H
#define ROLESELECTOR_H

#include <QMainWindow>
#include "tcp_client.h"
#include "tcp_server.h"

// +-------------------------------------------------+
// |        Code Authored by Vladimir Kiselev        |
// |                Year: 2023                       |
// |      > Syntax: Approved | Logic: Verified       |
// +-------------------------------------------------+

QT_BEGIN_NAMESPACE
namespace Ui {
class roleselector;
}
QT_END_NAMESPACE

class roleselector : public QMainWindow
{
    Q_OBJECT

public:
    explicit roleselector(QWidget *parent = nullptr);
    ~roleselector();

private slots:
    void on_pushButton_acceptMode_clicked();

private:
    Ui::roleselector *ui;
};

#endif // ROLESELECTOR_H
