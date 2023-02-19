#include "dialoglist.h"
#include "../ui_dialoglist.h"
#include <QMessageBox>
#include <QDebug>
#include <QGridLayout>

DialogList::DialogList(QWidget *parent,vector<User> *friendList, vector<Group> *groupList) :
    QWidget(parent),
    currentUserFriendList(friendList),
    currentUserGroupList(groupList),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    setWindowTitle("QQ 2017");
    setWindowIcon(QPixmap(":/Image/down.png"));

    QGridLayout *friendLayout = new QGridLayout(ui->pagefriends);
    ui->pagefriends->setLayout(friendLayout);
    for(const auto & it : *currentUserFriendList){
        QToolButton * btn = new QToolButton(ui->pagefriends);
        btn->setText(it.getName().c_str());
        btn->setIcon(QPixmap(":/Image/butterfly.png"));
        btn->setIconSize(QPixmap(":/Image/butterfly.png").size());
        //设置按钮风格 透明
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //加到垂直布局中
        friendLayout->addWidget(btn);
        tuple<QToolButton*, Dialog*> flag =  std::make_tuple(btn, nullptr);
        flags.emplace_back(flag);
        qDebug() << "123";
    }

    QGridLayout *groupLayout = new QGridLayout(ui->pagegroups);
    ui->pagegroups->setLayout(groupLayout);
    for(const auto &it : *currentUserGroupList){
        QToolButton * btn = new QToolButton(ui->pagegroups);
        btn->setText(it.getName().c_str());
        btn->setIcon(QPixmap(":/Image/butterfly.png"));
        btn->setIconSize(QPixmap(":/Image/butterfly.png").size());
        //设置按钮风格 透明
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //加到垂直布局中
        groupLayout->addWidget(btn);
        tuple<QToolButton*, Dialog*> flag =  std::make_tuple(btn, nullptr);
        flags.emplace_back(flag);
    }

    for(int i = 0; i < flags.size(); ++i){
        connect(std::get<0>(flags[i]), &QToolButton::clicked, this, [=]() {
            if(std::get<1>(flags[i])){
                emit std::get<1>(flags[i])->reshow();
                qDebug() << "raise";
                return;
            }
            Dialog *dialog = new Dialog(nullptr, std::get<0>(flags[i])->text());
            dialog->setWindowTitle(std::get<0>(flags[i])->text());
            dialog->setWindowIcon(std::get<0>(flags[i])->icon());
            std::get<1>(flags[i]) = dialog;
            connect(dialog, &Dialog::reshow, dialog, [=](){
                std::get<1>(flags[i])->raise();
                //让最小化或最大化的widget正常显示
                std::get<1>(flags[i])->showNormal();
            });
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            //关闭窗口后置空,以防下次无法打开
            connect(dialog, &Dialog::destroyed, this, [=](){
                std::get<1>(flags[i]) = nullptr;
                qDebug() << "close";
            });
            qDebug() << "new chat";
            dialog->show();
        });
    }

}


void DialogList::setFriendList(std::vector<User> *&& vec)
{
    currentUserFriendList = vec;
}

void DialogList::setGroupList(std::vector<Group> *&& vec)
{
    currentUserGroupList = vec;
}

DialogList::~DialogList()
{
    delete ui;
}
