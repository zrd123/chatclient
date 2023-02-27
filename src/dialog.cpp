#include "dialog.h"
#include "../ui_dialog.h"
#include <chrono>
#include "dialoglist.h"


Dialog::Dialog(QWidget *parent, QTcpSocket *tcp, const uint32_t &userid, const QString &name) :
    QWidget(parent),
    tcp(tcp),
    id(userid),
    username(name),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowIcon(QPixmap(":/Image/down.png"));

//    connect(tcp, &QTcpSocket::readyRead, this, [=](){
//        QByteArray data = tcp->readAll();
//        ui->msgBrowser->append(username+ "say: " + data);
//    });

    connect(this, &Dialog::oneChatMsg, this, &Dialog::doOneChat);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_sendBtn_clicked()
{
    QString msg = ui->msgTextEdit->toPlainText();

    chat_proto::ChatMessage cmsg;
    cmsg.set_type(chat_proto::ONE_CHAT_MSG);
    chat_proto::OneChatRequest ocRst;
    ocRst.set_to_id(id);
    ocRst.set_type(chat_proto::TEXT_TYPE);
    ocRst.set_user_id(DialogList::getUserId());
//    qDebug() << ocRst.to_id() << " " << ocRst.type() << " " << ocRst.user_id();
    ocRst.set_time(getCurrentTime().toStdString());
//    ocRst.set_chat_message(msg.toUtf8().data());
    ocRst.set_chat_message(msg.toStdString().c_str());
    qDebug() << QString(ocRst.time().c_str()) << "  " << QString(ocRst.chat_message().c_str());
    std::string *ocmsgbody= cmsg.mutable_message_body();
    ocRst.SerializeToString(ocmsgbody);
    std::string chatmsg;
    cmsg.SerializeToString(&chatmsg);
    qDebug() << "----";
    if(tcp->write(chatmsg.c_str()) <= 0){
        qDebug("write failed!");
    }
    qDebug() << QString(chatmsg.c_str());
    ui->msgBrowser->append("I say at " + QString(ocRst.time().c_str()) +":\n"+ msg);
    ui->msgTextEdit->clear();
}

void Dialog::doOneChat(chat_proto::OneChatRequest &ocRst)
{
    std::string msg(ocRst.time()+ ":\n" + ocRst.chat_message());
    qDebug() << QString(msg.c_str());
    ui->msgBrowser->append(msg.c_str());
}

QString Dialog::getCurrentTime() const
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return QString(date);
}

void Dialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

