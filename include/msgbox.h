//#ifndef MsgBox_H
//#define MsgBox_H

//#include <QWidget>


//#include <QThread>
//#include <QMessageBox>
//class MsgBoxThread : public QThread
//{
//    Q_OBJECT
//    typedef enum{
//        mbt_about = 0,
//        mbt_aboutqt = 1,
//        mbt_critical = 2,
//        mbt_information = 3,
//        mbt_question = 4,
//        mbt_warning = 5
//    } MsgBoxTYPE;
//public:
//    int m_btnres;
//    void about(QWidget * parent, const QString &title, const QString &text){
//        emit MsgBox_sig(mbt_about, parent, title, text, QMessageBox::NoButton, QMessageBox::NoButton);
//    }

//    void aboutQt(QWidget *parent, const QString &title = QString()){
//        emit MsgBox_sig(mbt_aboutqt, parent, title, tr(""), QMessageBox::NoButton, QMessageBox::NoButton);
//    }

//    int critical(QWidget * parent,
//                 const QString &title,
//                 const QString &text,
//                 QMessageBox::StandardButtons buttons = QMessageBox::Ok,
//                 QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton) {
//        emit MsgBox_sig(mbt_critical, parent, title, text, buttons, defaultButton);
//        return m_btnres;
//    }
//    int information(QWidget * parent,
//                    const QString &title,
//                    const QString &text,
//                    QMessageBox::StandardButtons buttons = QMessageBox::Ok,
//                    QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton){
//        emit MsgBox_sig(mbt_information, parent, title, text, buttons, defaultButton);
//        return m_btnres;
//    }
//    int question(QWidget * parent,
//                 const QString &title,
//                 const QString &text,
//                 QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
//                 QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton){
//        emit MsgBox_sig(mbt_question, parent, title, text, buttons, defaultButton);
//        return m_btnres;
//    }
//    int warning(QWidget * parent,
//                const QString &title,
//                const QString &text,
//                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
//                QMessageBox::StandardButtons defaultButton = QMessageBox::NoButton){
//        emit MsgBox_sig(mbt_warning, parent, title, text, buttons, defaultButton);
//        return m_btnres;
//    }

//public:
//    MsgBoxThread(QObject *parent = 0)
//         :QThread(parent), m_btnres(QMessageBox::NoButton) {
//         qRegisterMetaType<QMessageBox::StandardButtons>("QMessageBox::StandardButtons");
//         connect(this, /*SIGNAL(MsgBox_sig(
//                                  MsgBoxTYPE, QWidget *,
//                                  const QString,
//                                  const QString,
//                                  QMessageBox::StandardButtons,
//                                  QMessageBox::StandardButtons)),
//                 SLOT(on_information(MsgBoxTYPE, QWidget *,
//                                     const QString ,
//                                     const QString,
//                                     QMessageBox::StandardButtons ,
//                                     QMessageBox::StandardButtons )),*/
//                 &MsgBoxThread::MsgBox_sig,
//                 this, &MsgBoxThread::on_MsgBox,
//                 Qt::BlockingQueuedConnection);
//    }
//signals:
//    void MsgBox_sig(MsgBoxTYPE type,
//                    QWidget * parent,
//                    const QString &title,
//                    const QString &text,
//                    QMessageBox::StandardButtons buttons,
//                    QMessageBox::StandardButtons defaultButton);
//private slots:
//    void on_MsgBox(MsgBoxTYPE type,
//                   QWidget * parent,
//                   const QString &title,
//                   const QString &text,
//                   QMessageBox::StandardButtons buttons,
//                   QMessageBox::StandardButtons defaultButton) {
//        switch(type) {
//        case mbt_about:
//            QMessageBox::about(parent, title, text);
//            break;
//        case mbt_aboutqt:
//            QMessageBox::aboutQt(parent, title);
//            break;
//        case mbt_critical:
//            m_btnres = QMessageBox::critical(parent, title, text, buttons, defaultButton);
//            break;
//        case mbt_information:
//            m_btnres = QMessageBox::information(parent, title, text, buttons, defaultButton);
//            break;
//        case mbt_question:
//            m_btnres = QMessageBox::question(parent, title, text, buttons, defaultButton);
//            break;
//        case mbt_warning:
//            m_btnres = QMessageBox::warning(parent, title, text, buttons, defaultButton);
//            break;
//        default:
//            Q_ASSERT_X(false,"QMessageBox in thread","invalid box type specified.");
//        }
//    }
//};

//#endif // MsgBox_H
#ifndef MsgBox_H
#define MsgBox_H

#include <QObject>
#include<QEventLoop>
#include<QMessageBox>
#include<QApplication>
#include<QTimer>
class MsgBox:public QObject
{
    Q_OBJECT
public:
    enum mMsgType : uint8_t{
        m_about = 0,
        m_aboutqt = 1,
        m_critical = 2,
        m_information = 3,
        m_question = 4,
        m_warning = 5
    };
    explicit MsgBox(QObject *parent = nullptr);
private:
    const QString title;
    const QString msg;
    mMsgType type;
    void readyShow(void);
public:
    MsgBox(const QString &title, const QString &msg,const mMsgType type);
    static void show(const QString &title,const QString &msg,const mMsgType type);
public slots:
    void onShow();
};

#endif // MsgBox_H
