#include "roleselector.h"
#include "tcp_server.h"

#include <QApplication>
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/styles/resources/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    roleselector w;
    w.show();
    return a.exec();
}
