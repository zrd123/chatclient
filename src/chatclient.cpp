#include "chatclient.h"
#include "../ui_chatclient.h"
#include <thread>
#include <QtConcurrent>
#include <QNetworkProxy>
#include <QAbstractSocket>
#include "msgbox.h"

//#define _MYDEBUG

ChatClient::ChatClient(QWidget *parent) :
    QWidget(parent),
    dialoglist(nullptr),
    reg(nullptr),
    sem(new Semaphore(0)),
    ui(new Ui::ChatClient)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    client->setSocketOption(QAbstractSocket::LowDelayOption, 1);

//    client->setProxy(QNetworkProxy::NoProxy);
    //连接服务器,不能开代理(否则无法连接局域网内地址)
    client->connectToHost(QHostAddress("192.168.138.138"), 8000);
    if (client->waitForConnected(3000))
        qDebug("Connected!");
    else{
        QMessageBox::warning(nullptr, "disconnected", "The network connection is failed!");
    }

    //处理socket的tcp连接及读取
    connect(client, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::StandardButton box =
                QMessageBox::information(this, "disconnected",
                    "The network has disconnected! Do you want to log in again?", QMessageBox::Yes|QMessageBox::No);
        if(box == QMessageBox::No){

        }
        else{
            client->connectToHost(QHostAddress("192.168.138.138"), 8000);
            if (client->waitForConnected(3000))
                qDebug("Connected!");
            else{
                QMessageBox::warning(this, "disconnected", "The network connection is failed!");
            }
        }
        qDebug("disconnected!");
    });
#ifdef _MYDEBUG
    qDebug() << "main thread" << QThread::currentThreadId();
#endif
    connect(client, &QTcpSocket::readyRead, this, [=](){
//        std::async(std::launch::async, [&](){
        QtConcurrent::run([&]() {
            if(client->bytesAvailable() <= 0){
#ifdef _MYDEBUG
                qDebug() << "no bytes!";
#endif
                return ;
            }
            std::string msg(client->readAll());
            chat_proto::ChatMessage cmsg;
            //将读取的信息进行反序列化得到protobuf对象，并进行处理
            cmsg.ParseFromString(msg);
#ifdef _MYDEBUG
            qDebug() << QString(msg.c_str());
            qDebug() << "in read connect:" << QThread::currentThreadId();
#endif
            readTaskHandler(cmsg);
        });
    });

    //处理登录情况
    //异步处理登录业务
    connect(ui->signinBtn, &QPushButton::clicked, this, [=](){
        if(!g_isLoginSuccess){
            //            std::async(std::launch::async, [&](){
            QtConcurrent::run([&]() {
                doLogin();
#ifdef _MYDEBUG
                qDebug() << "sign in thread:" <<QThread::currentThreadId();
#endif
                sem->wait();
#ifdef _MYDEBUG
                qDebug() << "waitting for read";
#endif
            });
        }
    });
    connect(this, &ChatClient::signIn, this, [&](
            std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *friendList,
            std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *groupList){
        disconnect(client, &QTcpSocket::readyRead, this, nullptr);
        connect(client, &QTcpSocket::readyRead, this, [=](){
            if(client->bytesAvailable() <= 0){
                qDebug() << "no bytes!";
                return ;
            }
            std::string msg(client->readAll());
            chat_proto::ChatMessage cmsg;
            //将读取的信息进行反序列化得到protobuf对象，并进行处理
            cmsg.ParseFromString(msg);
//#ifdef _MYDEBUG
            qDebug() << QString(msg.c_str());
            qDebug() << "in read connect:" << QThread::currentThreadId();
//#endif
            readTaskHandler(cmsg);
        });
        dialoglist = new DialogList(nullptr, client, friendList, groupList);
        sem->signal();
        this->hide();
        dialoglist->show();
    });
    connect(this, &ChatClient::signFail, this, [&](QString errmsg){
        QMessageBox::warning(nullptr, "warning", errmsg);
    });

    //注册页面的管理
    connect(ui->enrollBtn, &QPushButton::clicked, this, [=](){
        reg = new Register(nullptr, client);
        connect(reg, &Register::signIn, this, [&](){
            reg->hide();
            this->show();
        });
        connect(reg, &Register::finishRegister, this, [&](){
            reg->hide();
            this->show();
            delete reg;
            reg = nullptr;
        });

        this->hide();
        ui->userIdEdit->clear();
        ui->passwordEdit->clear();
        reg->show();
        reg->setAttribute(Qt::WA_DeleteOnClose);
    });

}

ChatClient::~ChatClient()
{
    google::protobuf::ShutdownProtobufLibrary();
    delete ui;
    delete client;
    delete sem;
    delete dialoglist;
    if(reg){
        delete reg;
    }
}

//关闭处理，取消连接失败窗口
void ChatClient::closeEvent(QCloseEvent *event)
{
    disconnect(client, &QTcpSocket::disconnected, nullptr, nullptr);
    event->accept();
}

//处理登录业务
void ChatClient::doLogin()
{
    uint32_t username = atoi(ui->userIdEdit->text().toStdString().c_str());
    std::string password = ui->passwordEdit->text().toStdString();
    chat_proto::ChatMessage cmsg;
    cmsg.set_type(chat_proto::LOGIN_MSG);
    chat_proto::LoginRequest loginRst;
    std::string *loginmsgbody = cmsg.mutable_message_body();
    loginRst.set_user_id(username);
    loginRst.set_password(password);
    loginRst.set_request_info(true);
    //序列化
    loginRst.SerializeToString(loginmsgbody);
    cmsg.set_length(loginmsgbody->size());
    std::string chatmsg;
    cmsg.SerializeToString(&chatmsg);

#ifdef _MYDEBUG
    qDebug() << QString(loginmsgbody->c_str());
#endif
    if(client->write(chatmsg.c_str()) <= 0){
        qDebug("write failed!");
    }
#ifdef _MYDEBUG
    qDebug() << "doLogin";
#endif
}

//处理登录响应业务
void ChatClient::doLoginResponse(chat_proto::ChatMessage &cmsg)
{
    chat_proto::LoginResponse loginRsp;
    if(!loginRsp.ParseFromString(cmsg.message_body())){
        QMessageBox::warning(nullptr, "warning", QString("parse error in login response"));
    }
    if (chat_proto::LOGIN_ERR_2 == loginRsp.error_status()){ // 登录失败
#ifdef _MYDEBUG
        qDebug() << "before message box";
#endif
        QString errmsg("user has signed in other places!");
        qDebug() << errmsg;
        emit signFail(errmsg);
#ifdef _MYDEBUG
        qDebug() << "login failed!";
#endif
        g_isLoginSuccess = false;
        ui->signinBtn->setText("");
    }
    else if(chat_proto::LOGIN_ERR_3 == loginRsp.error_status()){
        QString errmsg("user id or password is error!");
        qDebug() << errmsg;
        ui->signinBtn->setText("");
        emit signFail(errmsg);
    }
    else if(chat_proto::NO_ERR == loginRsp.error_status()){ // 登录成功
        g_isLoginSuccess = true;
        qDebug("login success!");

        auto friendList = new std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>>();
        auto groupList = new std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>>();
        // 记录当前用户的好友列表信息
        chat_proto::LoadResponse loadRsp(loginRsp.user_info());
        qDebug() << "加载信息";
        chat_proto::User *user = new chat_proto::User(loginRsp.user());

        DialogList::setUser(user);
        for(const auto & _friend : loadRsp.friend_list()){
            chat_proto::User *user = new chat_proto::User();
            if(user){
//                qDebug() << _friend.id();
                user->set_id(_friend.id());
                user->set_name(_friend.name());
                user->set_status(_friend.status());
                friendList->insert({user->id(), {user, nullptr, nullptr}});
            }
            else{

            }
        }

        for(const auto & _group : loadRsp.group_list()){
            chat_proto::Group *group = new chat_proto::Group();
            if(group){
//                qDebug() << _group.id();
                group->set_id(_group.id());
                group->set_name(_group.name());
                group->set_description(_group.description());
//                qDebug() << QString(group->name().c_str()) << "---members:" << _group.members_size();
                for(const auto & _member : _group.members()){
                    auto member = group->add_members();
//                    qDebug() << QString(_member.user().name().c_str()) << " " << _member.user().id();
                    *member = _member;
                }
                groupList->insert({group->id(), {group, nullptr, nullptr}});
            }
            else{

            }
        }
        emit signIn(friendList, groupList);
    }
}

//处理读取的protobuf对象，并进一步解析
void ChatClient::readTaskHandler(chat_proto::ChatMessage &cmsg)
{
    // 接收ChatServer转发的数据，根据消息类型进行处理
    chat_proto::MessageType msgtype = cmsg.type();
    qDebug() << "msg type :" << msgtype << cmsg.ByteSizeLong();
    if (chat_proto::ONE_CHAT_MSG == msgtype){
        sem->wait();
        if(dialoglist){
            qDebug() << "one chat";
            emit dialoglist->oneChat(cmsg);
        }
    }
    else if (chat_proto::GROUP_CHAT_MSG == msgtype){
        qDebug() << "group chat";
        emit dialoglist->groupChat(cmsg);
    }
    else if (chat_proto::LOGIN_MSG_ACK == msgtype){
        doLoginResponse(cmsg);    // 处理登录响应的业务逻辑
#ifdef _MYDEBUG
        qDebug() << "read login task!";
#endif
        sem->signal();           // 通知主线程，登录结果处理完成
//        sem_post(&rwsem);
    }
    else if (chat_proto::REG_MSG_ACK == msgtype){
        reg->doRegisterResponse(cmsg);
#ifdef _MYDEBUG
        qDebug() << "read reg task!";
#endif
    }
    else if (chat_proto::ADD_FRIEND_ACK == msgtype){

    }
    else if (chat_proto::CREATE_GROUP_ACK == msgtype){

    }
    else if (chat_proto::ADD_GROUP_ACK == msgtype){

    }
    else{
        MsgBox::show("do parse", QString("unknown error!"), MsgBox::m_warning);
    }
}
