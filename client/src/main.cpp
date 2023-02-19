#include "mainwindow.h"
#include "dialoglist.h"
#include "dialog.h"
#include "chatclient.h"
#include <QApplication>
#include <unistd.h>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkProxy>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    ChatClient clinet;
    clinet.show();
//    QTcpServer server;
//    server.listen(QHostAddress::Any, 8000);



    return a.exec();
}
