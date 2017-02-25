#ifndef FAS_EVENTLOOPTHREAD_H
#define FAS_EVENTLOOPTHREAD_H


#include <Mutex.h>
#include <Condition.h>
#include <Thread.h>


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class EventLoop;

class EventLoopThread : boost::noncopyable {
public:
    EventLoopThread();
    EventLoopThread(const std::string& name);

    bool join();
    bool MainThread();

    EventLoop *start();

    void setName(const std::string name);
    std::string getName();

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

