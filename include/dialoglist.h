#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>
#include <tuple>
#include <QToolButton>
#include <QTcpSocket>
#include <QCloseEvent>
#include "dialog.h"
#include "others.pb.h"
#include <vector>
#include <unordered_map>

using namespace chat_proto;

namespace Ui {
class DialogList;
}

class DialogList : public QWidget
{
    Q_OBJECT
public:
    explicit DialogList(
            QWidget *parent = nullptr,
            QTcpSocket *tcp = nullptr,
            std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *friendList = nullptr,
            std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *groupList = nullptr);
    void setFriendList(std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>>* &&);
    void setGroupList(std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>>* &&);
    ~DialogList();

    static uint32_t getUserId(){ if(currentUser) return currentUser->id(); return 0;}
    static void setUser(chat_proto::User *user);


signals:
    void oneChat(chat_proto::ChatMessage &cmsg);
    void groupChat(chat_proto::ChatMessage &cmsg);
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void doOneChat(chat_proto::ChatMessage &cmsg);
    void doGroupChat(chat_proto::ChatMessage &cmsg);

private:
    Dialog *newDialog(std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>>::iterator &it);
    Dialog *newDialog(std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>>::iterator &it);

private:
    Ui::DialogList *ui;
    QTcpSocket *tcp;
    static chat_proto::User *currentUser;
//    QVector<tuple<uint32_t, QToolButton*, Dialog*>> flags;
    std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *currentUserFriendList;
    std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *currentUserGroupList;
};


#endif // DIALOGLIST_H
