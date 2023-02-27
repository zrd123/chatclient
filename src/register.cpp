#include "register.h"
#include "../ui_register.h"
#include <QMessageBox>
#include <regex>
#include "msgbox.h"

Register::Register(QWidget *parent, QTcpSocket *client) :
    QWidget(parent),
    client(client),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    qDebug() << "new register window";

    connect(ui->r_signinBtn, &QPushButton::clicked, this, [=](){
        this->hide();
        emit signIn();
    });
    connect(ui->r_enrollBtn, &QPushButton::clicked, this, &Register::doRegister);
}

Register::~Register()
{
    client = nullptr;
    delete ui;
}

void Register::doRegisterResponse(chat_proto::ChatMessage &cmsg) const
{
    chat_proto::RegisterResponse regRsp;
    if(!regRsp.ParseFromString(cmsg.message_body())){
        MsgBox::show("register failed", QString("parse error in register response"), MsgBox::m_warning);
    }
    if(chat_proto::NO_ERR == regRsp.error_status()){
        uint32_t id = regRsp.user_id();
        MsgBox::show("register successfully", QString("please remember id %1").arg(id), MsgBox::m_information);
        emit finishRegister();
    }
    else if(chat_proto::REG_ERR_2 == regRsp.error_status()){
        MsgBox::show("register failed", QString("register error in register response"), MsgBox::m_warning);
    }
    else if(chat_proto::REG_ERR_1 == regRsp.error_status()){
        MsgBox::show("register failed", QString("unknown register error in register response"), MsgBox::m_warning);
    }
    else {
        MsgBox::show("register failed", QString("unknown error in register response"), MsgBox::m_warning);
    }
}

void Register::doRegister() const
{
    std::string username(ui->r_userNameEdit->text().toStdString());
    std::string passward(ui->r_passwordEdit->text().toStdString());
    std::string cfmpassward(ui->r_cpasswordEdit->text().toStdString());
    std::regex pattern("^(?!_)(?!.*?_$)[a-zA-Z0-9_\u4e00-\u9fa5]{4,25}$");
    if(!std::regex_match(username, pattern)){
        MsgBox::show("register failed", QString("Please enter the vaild name!"), MsgBox::m_warning);
    }
    if(passward == "" || passward != cfmpassward){
        MsgBox::show("register failed", QString("The two passwords do not match! Please re-enter!"), MsgBox::m_warning);
        ui->r_passwordEdit->setText("");
        ui->r_cpasswordEdit->setText("");
    }
    chat_proto::ChatMessage cmsg;
    cmsg.set_type(chat_proto::REG_MSG);
    chat_proto::RegisterRequest regmsg;
    regmsg.set_user_name(username.c_str());
    regmsg.set_password(passward.c_str());
    std::string *regmsgbody = cmsg.mutable_message_body();
    regmsg.SerializeToString(regmsgbody);
    std::string chatmsg;
    cmsg.SerializeToString(&chatmsg);
    if(client && client->write(chatmsg.c_str()) <= 0){
        qDebug("write failed!");
    }
}
