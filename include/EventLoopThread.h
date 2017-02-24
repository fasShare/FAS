#ifndef FAS_EVENTLOOPTHREAD_H
#define FAS_EVENTLOOPTHREAD_H
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <EventLoop.h>
#include <Mutex.h>
#include <Condition.h>
#include <Thread.h>

class EventLoopThread : boost::noncopyable {
public:
    EventLoopThread();
    EventLoopThread(const string& name);

    bool join();
    bool MainThread();

    EventLoop *start();
    bool stop();

    void setName(const string name);
    string getName();

    void threadFunc();

    ~EventLoopThread();

private:
    Mutex mutex_;
    Condition cond_;
    Thread thread_;
    EventLoop *loop_;
    bool started_;
};

#endif // FAS_EVENTLOOPTHREAD_H

