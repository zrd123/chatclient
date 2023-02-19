#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>
#include <tuple>
#include <QToolButton>
#include "dialog.h"
#include "user.hpp"
#include "group.hpp"
#include <vector>

using std::tuple;

namespace Ui {
class DialogList;
}

class DialogList : public QWidget
{
    Q_OBJECT

public:
    explicit DialogList(QWidget *parent = nullptr,vector<User> *friendList = nullptr, vector<Group> *groupList = nullptr);
    void setFriendList(std::vector<User>* &&);
    void setGroupList(std::vector<Group>* &&);
    ~DialogList();

private:
    Ui::DialogList *ui;
    User currentUser;
    QVector<tuple<QToolButton*, Dialog*>> flags;
    std::vector<User> *currentUserFriendList;
    std::vector<Group> *currentUserGroupList;


};

#endif // DIALOGLIST_H
