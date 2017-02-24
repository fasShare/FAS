#ifndef FAS_EVENTLOOPTHREADPOOL_H
#define FAS_EVENTLOOPTHREADPOOL_H
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <EventLoopThread.h>
#include <EventLoop.h>
#include <vector>

using std::vector;

class EventLoopThreadPool : boost::noncopyable {
public:
    EventLoopThreadPool(EventLoop *baseloop, int threadNum);
    EventLoopThreadPool(EventLoop *baseloop, int threadNum, string name);
    ~EventLoopThreadPool();

    void updateThreadNum(int threadNum);
    int getThreadNum();

    void updateName(const string& name);
    string getName();

    void assertInOwner();

    bool start();

    EventLoop *getNextEventLoop();
private:
    int threadNum_;
    string name_;
    vector<EventLoopThread *> threads_;
    vector<EventLoop *> loops_;
    EventLoop *baseloop_;
    bool started_;
    size_t next_;
    pid_t tid_; //thread owner pid.
};

#endif // FAS_EVENTLOOPTHREADPOOL_H

