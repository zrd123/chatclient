#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QTcpSocket>
#include "others.pb.h"

class ChatClient;

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT
    friend class ChatClient;

public:
    explicit Register(QWidget *parent = nullptr, QTcpSocket *client = nullptr);
    ~Register();
signals:
    void signIn();
    void finishRegister() const;

private slots:
    void doRegisterResponse(chat_proto::ChatMessage &cmsg) const;
    void doRegister() const;

private:
    Ui::Register *ui;
    QTcpSocket *client;
};

#endif // REGISTER_H
