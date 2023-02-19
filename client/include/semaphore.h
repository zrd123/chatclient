#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <QDebug>

class Semaphore
{
public:
    Semaphore(long count = 0) : count(count) {}
    //V操作，唤醒
    void signal()
    {
        std::unique_lock<std::mutex> unique(mt);
        ++count;
        qDebug() << "signal in semaphore" << count;
        if (count <= 0){
            cond.notify_one();
        }
    }
    //P操作，阻塞
    void wait()
    {
        std::unique_lock<std::mutex> unique(mt);
        --count;
        if (count < 0){
            qDebug() << "wait in semaphore" << count;
            cond.wait(unique);
        }
    }

private:
    std::mutex mt;
    std::condition_variable cond;
    long count;
};

#endif // SEMAPHORE_H
