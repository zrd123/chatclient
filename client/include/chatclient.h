#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <atomic>
#include <string>
#include <QMessageBox>
#include "json.hpp"
#include "public.hpp"
#include "semaphore.h"
#include "dialoglist.h"


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
    void signIn(vector<User> *friendList, vector<Group> *groupList);
    void signFail(QString errmsg);

private:
    void doLogin();
    void doLoginResponse(json &responsejs);
    void doRegister() const;
    void doRegisterResponse(json &responsejs);
    void readTaskHandler(json &js);

    Ui::ChatClient *ui;
    QTcpSocket *client;
//    QTcpSocket *read;
    Semaphore *sem;
    std::atomic_bool g_isLoginSuccess{false};
    DialogList *dialoglist;
};

#endif // CHATCLIENT_H
