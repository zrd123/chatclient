#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Dialog;
}

class Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent, QString name);
    ~Dialog();
signals:
//    bool isShow() const;
//    void setShow(bool flag);
    void reshow();

private slots:
    void on_sendBtn_clicked();

private:
    Ui::Dialog *ui;
    QTcpSocket *tcp;
    QString username;
//    bool isshow = true;
};

#endif // DIALOG_H
