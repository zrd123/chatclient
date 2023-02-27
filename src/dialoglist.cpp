#include "dialoglist.h"
#include "../ui_dialoglist.h"
#include <QMessageBox>
#include <QDebug>
#include <QGridLayout>
#include <QToolButton>
#include "msgbox.h"

chat_proto::User *DialogList::currentUser;

DialogList::DialogList(QWidget *parent,
                       QTcpSocket *tcp,
                       std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *friendList,
                       std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *groupList) :
    QWidget(parent),
    tcp(tcp),
    currentUserFriendList(friendList),
    currentUserGroupList(groupList),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    setWindowTitle("QQ 2017");
    setWindowIcon(QPixmap(":/Image/down.png"));

    connect(this, &DialogList::oneChat, this, &DialogList::doOneChat);
    connect(this, &DialogList::groupChat, this, &DialogList::doGroupChat);

    connect(ui->addFriendTBtn, &QToolButton::clicked, this, [=](){
        chat_proto::ChatMessage cmsg;
        cmsg.set_type(chat_proto::ADD_FRIEND_MSG);
        chat_proto::AddFriendRequest afRst;
        std::string friendId(ui->r_cpasswordEdit->text().toStdString());
        std::regex pattern("^[0-9]");
        if(!std::regex_match(friendId, pattern)){
            MsgBox::show("add friend failed", QString("Please enter the vaild id!"), MsgBox::m_warning);
            return ;
        }
        afRst.set_user_id(DialogList::getUserId());
        afRst.set_friend_id(std::stoul(friendId));
        QMessageBox::StandardButton box =
                QMessageBox::information(this, "close",
                                         "Are you sure to quit?", QMessageBox::Yes|QMessageBox::No);
        if(box == QMessageBox::No){
        }
        else{
            std::string *afmsgbody = cmsg.mutable_message_body();
            afRst.SerializeToString(afmsgbody);
            std::string afmsg;
            cmsg.SerializeToString(&afmsg);
            if(tcp->write(afmsg.c_str()) <= 0){
                qDebug("write failed in add friend request!");
            }
        }
    });


    QGridLayout *friendLayout = new QGridLayout(ui->pagefriends);
    ui->pagefriends->setLayout(friendLayout);
    if(currentUserFriendList)
    for(auto & it : *currentUserFriendList){
        QToolButton * btn = new QToolButton(ui->pagefriends);
        btn->setText((std::get<0>(it.second))->name().c_str());
        btn->setIcon(QPixmap(":/Image/butterfly.png"));
        btn->setIconSize(QPixmap(":/Image/butterfly.png").size());
        //设置按钮风格 透明
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //加到垂直布局中
        friendLayout->addWidget(btn);
        std::get<1>(it.second) = btn;
        Dialog *dialog = new Dialog(nullptr, this->tcp, it.first, std::get<0>(it.second)->name().c_str());
        dialog->setWindowTitle(std::get<0>(it.second)->name().c_str());
        dialog->setWindowIcon(std::get<1>(it.second)->icon());
        std::get<2>(it.second) = dialog;
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        //关闭窗口后置空,以防下次无法打开
        connect(dialog, &Dialog::destroyed, this, [&](){
            std::get<2>(it.second) = nullptr;
            qDebug() << "close";
        });
        connect(btn, &QToolButton::clicked, this, [&](){
            if(std::get<2>(it.second)){
                std::get<2>(it.second)->raise();
                //让最小化或最大化的widget正常显示
                std::get<2>(it.second)->showNormal();
                qDebug() << "raise";
                return;
            }
            Dialog *dialog = new Dialog(nullptr, this->tcp, it.first, std::get<0>(it.second)->name().c_str());
            dialog->setWindowTitle(std::get<0>(it.second)->name().c_str());
            dialog->setWindowIcon(std::get<1>(it.second)->icon());
            std::get<2>(it.second) = dialog;
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            //关闭窗口后置空,以防下次无法打开
            connect(dialog, &Dialog::destroyed, this, [&](){
                std::get<2>(it.second) = nullptr;
                qDebug() << "close";
            });
            qDebug() << "new chat";
            dialog->show();
        });
    }

    QGridLayout *groupLayout = new QGridLayout(ui->pagegroups);
    ui->pagegroups->setLayout(groupLayout);
    if(currentUserGroupList)
    for(auto &it : *currentUserGroupList){
        QToolButton * btn = new QToolButton(ui->pagegroups);
        btn->setText(std::get<0>(it.second)->name().c_str());
        btn->setIcon(QPixmap(":/Image/butterfly.png"));
        btn->setIconSize(QPixmap(":/Image/butterfly.png").size());
        //设置按钮风格 透明
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //加到垂直布局中
        groupLayout->addWidget(btn);
        std::get<1>(it.second) = btn;
        Dialog *dialog = new Dialog(nullptr, this->tcp, it.first, std::get<0>(it.second)->name().c_str());
        dialog->setWindowTitle(std::get<0>(it.second)->name().c_str());
        dialog->setWindowIcon(std::get<1>(it.second)->icon());
        std::get<2>(it.second) = dialog;
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        //关闭窗口后置空,以防下次无法打开
        connect(dialog, &Dialog::destroyed, this, [&](){
            std::get<2>(it.second) = nullptr;
            qDebug() << "close";
        });
        connect(btn, &QToolButton::clicked, this, [&]() {
            if(std::get<2>(it.second)){
                //让该窗口置于顶层
                std::get<2>(it.second)->raise();
                //让最小化或最大化的widget正常显示
                std::get<2>(it.second)->showNormal();
                qDebug() << "raise";
                return;
            }
            Dialog *dialog = new Dialog(nullptr, tcp, it.first, std::get<0>(it.second)->name().c_str());
            dialog->setWindowTitle(std::get<0>(it.second)->name().c_str());
            dialog->setWindowIcon(std::get<1>(it.second)->icon());
            std::get<2>(it.second) = dialog;
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            //关闭窗口后置空,以防下次无法打开
            connect(dialog, &Dialog::destroyed, this, [&](){
                std::get<2>(it.second) = nullptr;
                qDebug() << "close";
            });
            qDebug() << "new chat";
            dialog->show();
        });
    }
    qDebug() << "initialize dialoglist";
}

Dialog *DialogList::newDialog(std::unordered_map<uint32_t, std::tuple<User *, QToolButton *, Dialog *> >::iterator &it)
{
    Dialog *dialog = new Dialog(nullptr, tcp, it->first, std::get<0>(it->second)->name().c_str());
    dialog->setWindowTitle(std::get<0>(it->second)->name().c_str());
    dialog->setWindowIcon(std::get<1>(it->second)->icon());
//    std::get<2>(it.second) = dialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    //关闭窗口后置空,以防下次无法打开
    connect(dialog, &Dialog::destroyed, this, [&](){
        std::get<2>(it->second) = nullptr;
        qDebug() << "close";
    });
    return dialog;
}

Dialog *DialogList::newDialog(std::unordered_map<uint32_t, std::tuple<Group *, QToolButton *, Dialog *> >::iterator &it)
{
    Dialog *dialog = new Dialog(nullptr, tcp, it->first, std::get<0>(it->second)->name().c_str());
    dialog->setWindowTitle(std::get<0>(it->second)->name().c_str());
    dialog->setWindowIcon(std::get<1>(it->second)->icon());
//    std::get<2>(it.second) = dialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    //关闭窗口后置空,以防下次无法打开
    connect(dialog, &Dialog::destroyed, this, [&](){
        std::get<2>(it->second) = nullptr;
        qDebug() << "close";
    });
    return dialog;
}

void DialogList::addFriend()
{

}

void DialogList::addGroup()
{

}



void DialogList::setFriendList(std::unordered_map<uint32_t, std::tuple<chat_proto::User*, QToolButton*, Dialog*>> *&& vec)
{
    currentUserFriendList = vec;
}

void DialogList::setGroupList(std::unordered_map<uint32_t, std::tuple<chat_proto::Group*, QToolButton*, Dialog*>> *&& vec)
{
    currentUserGroupList = vec;
}

DialogList::~DialogList()
{
    for(auto &it : *currentUserFriendList){
        if(std::get<1>(it.second)){
            delete std::get<1>(it.second);
            std::get<1>(it.second) = nullptr;
        }
        if(std::get<2>(it.second)){
            delete std::get<2>(it.second);
            std::get<2>(it.second) = nullptr;
        }

    }
    delete currentUserFriendList;
    for(auto &it : *currentUserGroupList){
        if(std::get<1>(it.second)){
            delete std::get<1>(it.second);
            std::get<1>(it.second) = nullptr;
        }
        if(std::get<2>(it.second)){
            delete std::get<2>(it.second);
            std::get<2>(it.second) = nullptr;
        }
    }
    delete currentUserGroupList;
    delete ui;
}

void DialogList::setUser(User *user)
{
    currentUser = user;
}

void DialogList::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton box =
            QMessageBox::information(this, "close",
                                     "Are you sure to quit?", QMessageBox::Yes|QMessageBox::No);
    if(box == QMessageBox::No){
        event->ignore();
    }
    else{
        disconnect(tcp, &QTcpSocket::disconnected, nullptr, nullptr);
        event->accept();
    }
}

void DialogList::doOneChat(ChatMessage &cmsg)
{
    chat_proto::OneChatRequest ocRst;
    if(!ocRst.ParseFromString(cmsg.message_body())){
        MsgBox::show("Parse failed", QString("Parse error in on chat response"), MsgBox::m_warning);
    }
    auto info = currentUserFriendList->at(ocRst.user_id());
    qDebug() << "list do one chat" << std::get<0>(info)->id();
    emit std::get<2>(info)->oneChatMsg(ocRst);
}

void DialogList::doGroupChat(ChatMessage &cmsg)
{

}


