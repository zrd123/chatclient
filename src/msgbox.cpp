#include "msgBox.h"

MsgBox::MsgBox(QObject *parent):QObject (parent)
{

}

void MsgBox::readyShow()
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0,this,SLOT(onShow()));
}

MsgBox::MsgBox(const QString &title, const QString &msg,const mMsgType type)
    : title(title), msg(msg), type(type)
{

}

void MsgBox::show(const QString &title, const QString &msg,const mMsgType type)
{
    QEventLoop eventLoop;
    auto messageBox = new MsgBox(title,msg,type);
    connect(messageBox, &MsgBox::destroyed, &eventLoop, &QEventLoop::quit);
    messageBox->readyShow();
    eventLoop.exec();
}

void MsgBox::onShow()
{
    switch (type)
    {
    case m_information:
        QMessageBox::information(nullptr, title, msg);
        break;
    case m_critical:
        QMessageBox::critical(nullptr, title, msg);
        break;
    case m_question:
        QMessageBox::question(nullptr, title, msg);
        break;
    case m_about:
        QMessageBox::about(nullptr, title, msg);
        break;
    case m_aboutqt:
        QMessageBox::aboutQt(nullptr, title);
        break;
    case m_warning:
        QMessageBox::warning(nullptr, title, msg);
    }
    this->deleteLater();
}
