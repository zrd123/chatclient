#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <atomic>
#include <string>
#include <QMessageBox>
#include <QCloseEvent>
#include "json.hpp"
#include "semaphore.h"
#include "dialoglist.h"
#include "others.pb.h"
#include "register.h"

namespace Ui {
class ChatClient;
}

class ChatClient : public QWidget
{
    Q_OBJECT

public:
    using json = nlohmann::json;

    explicit ChatClient(QWidget *parent = nullptr);
    ~ChatClient();

signals:
    void signIn(std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *friendList,
                std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *groupList);
    void signFail(QString errmsg);
    void doRegister(chat_proto::ChatMessage &cmsg);
protected:
    void closeEvent(QCloseEvent *event);
private:
    void doLogin();
    void doLoginResponse(json &responsejs);
    void doLoginResponse(chat_proto::ChatMessage &cmsg);
//    void doRegister() const;
//    void doRegisterResponse(json &responsejs){}
//    void readTaskHandler(json &js);
    void readTaskHandler(chat_proto::ChatMessage &cmsg);

    Ui::ChatClient *ui;
    QTcpSocket *client;
    Register *reg;
//    QTcpSocket *read;
    Semaphore *sem;
    std::atomic_bool g_isLoginSuccess{false};
    DialogList *dialoglist;
};

#endif // CHATCLIENT_H
