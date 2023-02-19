#include "chatclient.h"
#include "../ui_chatclient.h"
#include <thread>
#include <QThread>
#include <QtConcurrent>

//#define _myDebug

ChatClient::ChatClient(QWidget *parent) :
    QWidget(parent),
    dialoglist(nullptr),
    sem(new Semaphore(0)),
    ui(new Ui::ChatClient)
{
    ui->setupUi(this);
    client = new QTcpSocket;
//    read = new QTcpSocket(this);
    //client.setProxy(QNetworkProxy::NoProxy);
    //连接服务器,不能开代理(否则无法连接局域网内地址)
    client->connectToHost(QHostAddress("192.168.138.138"), 8000);

    if (client->waitForConnected(3000))
        qDebug("Connected!");
    else{
        qDebug("failed");
    }

    connect(client, &QTcpSocket::disconnected, this, [=](){
        qDebug("disconnected!");
    });
#ifdef _myDebug
    qDebug() << "main thread" << QThread::currentThreadId();
#endif
    connect(client, &QTcpSocket::readyRead, this, [=](){
//        std::async(std::launch::async, [&](){
        QtConcurrent::run([&]() {
            if(client->bytesAvailable() <= 0){
#ifdef _myDebug
                qDebug() << "no bytes!";
#endif
                return ;
            }
            auto response = client->readAll();
            json js = json::parse(response.data());
#ifdef _myDebug
            qDebug() << response;
            qDebug() << "in read connect:" << QThread::currentThreadId();
#endif
            readTaskHandler(js);
        });
    });

    connect(ui->signinBtn, &QPushButton::clicked, this, [=](){
        if(!g_isLoginSuccess){
            QtConcurrent::run([&]() {
                doLogin();
#ifdef _myDebug
                qDebug() << "sign in thread:" <<QThread::currentThreadId();
#endif
                sem->wait();
#ifdef _myDebug
                qDebug() << "waitting for read";
#endif
            });
        }
    });
    connect(this, &ChatClient::signIn, this, [&](
            vector<User> *friendList,
            vector<Group> *groupList){
        dialoglist = new DialogList(nullptr, friendList, groupList);
        this->hide();
        dialoglist->show();
    });
    connect(this, &ChatClient::signFail, this, [&](QString errmsg){
        QMessageBox::warning(nullptr, "warning", errmsg);
    });
}

ChatClient::~ChatClient()
{
    delete ui;
    delete client;
    delete sem;
    delete dialoglist;
}

void ChatClient::doLogin()
{
    int username = atoi(ui->userNameEdit->text().toStdString().c_str());
    std::string password = ui->passwordEdit->text().toStdString();
    json js;
    js["msgid"] = LOGIN_MSG;
    js["id"] = username;
    js["password"] = password;
    std::string request = js.dump();
#ifdef _myDebug
    qDebug() << QString(request.c_str());
#endif
    if(client->write(request.c_str()) <= 0){
        qDebug("write failed!");
    }
#ifdef _myDebug
    qDebug() << "doLogin";
#endif
}

void ChatClient::doRegister() const
{

}

void ChatClient::doRegisterResponse(json &)
{

}

void ChatClient::readTaskHandler(json& js)
{
    // 接收ChatServer转发的数据，反序列化生成json数据对象
    int msgtype = js["msgid"].get<int>();
    if (ONE_CHAT_MSG == msgtype){
//        cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
//                                         << " said: " << js["msg"].get<string>() << endl;
    }
    else if (GROUP_CHAT_MSG == msgtype){
//        cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
//             << " said: " << js["msg"].get<string>() << endl;
    }
    else if (LOGIN_MSG_ACK == msgtype){
        doLoginResponse(js);    // 处理登录响应的业务逻辑
#ifdef _myDebug
        qDebug() << "read task!";
#endif
        sem->signal();           // 通知主线程，登录结果处理完成
//        sem_post(&rwsem);
    }
    else if (REG_MSG_ACK == msgtype){
        doRegisterResponse(js);
        sem->signal();           // 通知主线程，注册结果处理完成
//        sem_post(&rwsem);
    }
    else{
        QMessageBox::warning(nullptr, "warning", QString("unknown error!"));
    }
}

void ChatClient::doLoginResponse(json &responsejs)
{
    if (0 != responsejs["errno"].get<int>()){ // 登录失败
#ifdef _myDebug
        qDebug() << "before message box";
#endif
        QString errmsg(responsejs["errmsg"].get<std::string>().c_str());
        qDebug() << errmsg;
        emit signFail(errmsg);
#ifdef _myDebug
        qDebug() << "login failed!";
#endif
        g_isLoginSuccess = false;
    }
    else{ // 登录成功
        g_isLoginSuccess = true;
        qDebug("login success!");

        vector<User> *friendList = nullptr;
        vector<Group> *groupList = nullptr;
        // 记录当前用户的好友列表信息
        if(responsejs.contains("friends")){
            friendList = new vector<User>();
            vector<std::string> vec= responsejs["friends"];
            for(std::string &friendstr : vec){
                json js = json::parse(friendstr);
                User user;
                user.setId(js["id"].get<int>());
                user.setName(js["name"]);
                user.setState(js["state"]);
                friendList->emplace_back(user);
            }
            //dialoglist->setFriendList(std::move(friendList));
            qDebug("loaded friendlist");
        }
        // 记录当前用户的群组列表信息
        if (responsejs.contains("groups")){
            groupList = new vector<Group>();
            vector<std::string> vec= responsejs["groups"];
            for (std::string &groupstr : vec){
                json grpjs = json::parse(groupstr);
                Group group;
                group.setId(grpjs["id"].get<int>());
                group.setName(grpjs["groupname"]);
                group.setDesc(grpjs["groupdesc"]);

                vector<string> vec2 = grpjs["users"];
                for (string &userstr : vec2){
                    GroupUser user;
                    json js = json::parse(userstr);
                    user.setId(js["id"].get<int>());
                    user.setName(js["name"]);
                    user.setState(js["state"]);
                    user.setRole(js["role"]);
                    group.getUsers().emplace_back(user);
                }
                groupList->emplace_back(group);
            }
            //dialoglist->setGroupList(std::move(groupList));
            qDebug("loaded grouplist");
        }

        emit signIn(friendList, groupList);
    }
}
