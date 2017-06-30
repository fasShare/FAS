#ifndef FAS_EVENTLOOPTHREADPOOL_H
#define FAS_EVENTLOOPTHREADPOOL_H
#include <vector>


#include <EventLoopThread.h>


#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace fas {

class EventLoop;
/*!
 * \brief The EventLoopThreadPool class
 * ThreadPool of EventLoopThread.
 */
class EventLoopThreadPool : boost::noncopyable {
public:
    EventLoopThreadPool(EventLoop *baseloop, int threadNum, std::string name = "EventLoopThreadPool");
    ~EventLoopThreadPool();

    void updateThreadNum(int threadNum);
    int getThreadNum();

    void updateName(const std::string& name);
    std::string getName();

    void assertInOwnerThread();

    bool start();
    bool stop();

    EventLoop *getNextEventLoop();
private:
    int threadNum_;
    std::string name_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
    EventLoop *baseloop_;
    bool started_;
    size_t next_;
    pid_t tid_; //thread owner pid.
};

}
#endif // FAS_EVENTLOOPTHREADPOOL_H

