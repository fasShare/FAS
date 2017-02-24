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

    void setThreadNum(int threadNum);
    int getThreadNum();

    void setName(const string& name);
    string getName();

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
};

#endif // FAS_EVENTLOOPTHREADPOOL_H

