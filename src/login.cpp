#include "login.h"
#include "../ui_login.h"
#include "public.hpp"
#include <QLineEdit>


Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    client = new QTcpSocket(this);
    //client.setProxy(QNetworkProxy::NoProxy);
    //连接服务器,不能开代理(否则无法连接局域网内地址)
    client->connectToHost(QHostAddress("192.168.0.103"), 8000);
    if (client->waitForConnected(3000))
        qDebug("Connected!");
    else{
        qDebug("failed");
    }

    connect(ui->signinBtn, &QPushButton::clicked, this, [=](){
        doLogin();
    });
}

Login::~Login()
{
    delete ui;
    delete client;
}

void Login::doLogin() const
{
    std::string username = ui->userNameEdit->text().toStdString();
    std::string password = ui->passwordEdit->text().toStdString();

    json js;
    js["msgid"] = LOGIN_MSG;
    js["id"] = username;
    js["password"] = password;
    std::string request = js.dump();

    client->write(request.c_str());
}

void Login::doLoginResponse(json &responsejs)
{
    if (0 != responsejs["errno"].get<int>()){ // 登录失败
        QMessageBox::warning(nullptr, "warning", QString(responsejs["errmsg"].get<std::string>().c_str()));
        g_isLoginSuccess = false;
    }
    else{ // 登录成功
        // 记录当前用户的id和name
        g_currentUser.setId(responsejs["id"].get<int>());
        g_currentUser.setName(responsejs["name"]);

        // 记录当前用户的好友列表信息
        if (responsejs.contains("friends")){
            // 初始化
            g_currentUserFriendList.clear();

            vector<string> vec = responsejs["friends"];
            for (string &str : vec){
                json js = json::parse(str);
                User user;
                user.setId(js["id"].get<int>());
                user.setName(js["name"]);
                user.setState(js["state"]);
                g_currentUserFriendList.push_back(user);
            }
        }

        // 记录当前用户的群组列表信息
        if (responsejs.contains("groups")){
            // 初始化
            g_currentUserGroupList.clear();

            vector<string> vec1 = responsejs["groups"];
            for (string &groupstr : vec1){
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
                    group.getUsers().push_back(user);
                }

                g_currentUserGroupList.push_back(group);
            }
        }

        // 显示登录用户的基本信息
        showCurrentUserData();

        // 显示当前用户的离线消息  个人聊天信息或者群组消息
        if (responsejs.contains("offlinemsg")){
            vector<string> vec = responsejs["offlinemsg"];
            for (string &str : vec){
                json js = json::parse(str);
                // time + [id] + name + " said: " + xxx
                if (ONE_CHAT_MSG == js["msgid"].get<int>()){
                    cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                                                     << " said: " << js["msg"].get<string>() << endl;
                }
                else{
                    cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                         << " said: " << js["msg"].get<string>() << endl;
                }
            }
        }

        g_isLoginSuccess = true;
    }
}
