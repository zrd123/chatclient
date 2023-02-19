#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>
#include <atomic>
#include <string>
#include <QMessageBox>
#include "json.hpp"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT
    using json = nlohmann::json;

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private:
    void doLogin() const;
    void doLoginResponse(json &responsejs);

    Ui::Login *ui;
    QTcpSocket *client;
    std::atomic_bool g_isLoginSuccess{false};

};

#endif // LOGIN_H
