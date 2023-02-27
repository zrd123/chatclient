#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QTcpSocket>
#include <QCloseEvent>
#include "others.pb.h"

namespace Ui {
class Dialog;
}

class DialogList;

class Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent, QTcpSocket *tcp, const uint32_t &userid, const QString &name);
    ~Dialog();
signals:
    void oneChatMsg(chat_proto::OneChatRequest &ocRst);
    void reshow();

private slots:
    void on_sendBtn_clicked();
    void doOneChat(chat_proto::OneChatRequest &ocRst);
protected:
    QString getCurrentTime() const;
    void closeEvent(QCloseEvent *event);
private:
    Ui::Dialog *ui;
    QTcpSocket *tcp;
    uint32_t id;
    QString username;
//    bool isshow = true;
};

#endif // DIALOG_H
