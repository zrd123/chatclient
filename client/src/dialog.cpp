#include "dialog.h"
#include "../ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString name) :
    QWidget(parent),
    username(name),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowIcon(QPixmap(":/Image/down.png"));
    tcp = new QTcpSocket(this);

    connect(tcp, &QTcpSocket::readyRead, this, [=](){
        QByteArray data = tcp->readAll();
        ui->msgBrowser->append(username+ "say: " + data);
    });

    connect(tcp, &QTcpSocket::disconnected, this, [=](){
        tcp->close();
        tcp->deleteLater();
        qDebug() << "disconnected!";
    });

    connect(tcp, &QTcpSocket::connected, this, [=](){
        qDebug() << "connect successed!";
    });
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_sendBtn_clicked()
{
    QString msg = ui->msgTextEdit->toPlainText();
    tcp->write(msg.toUtf8());
    ui->msgBrowser->append("I say: " + msg);
    ui->msgTextEdit->clear();
}

