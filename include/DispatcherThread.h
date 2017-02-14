#ifndef DISPATCHERTHREAD_H
#define DISPATCHERTHREAD_H
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <Dispatcher.h>
#include <Mutex.h>
#include <Condition.h>
#include <Thread.h>

class DispatcherThread : boost::noncopyable {
public:
    DispatcherThread();
    DispatcherThread(const string& name);

    bool join();
    bool MainThread();

    Dispatcher *start();
    bool stop();

    void setName(const string name);
    string getName();

    void threadFunc();

    ~DispatcherThread();

private:
    Mutex mutex_;
    Condition cond_;
    Thread thread_;
    Dispatcher *dispatch_;
    bool started_;
};

#endif // DISPATCHERTHREAD_H

