#ifndef FAS_EVENTLOOP_H
#define FAS_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <atomic>


#include <Mutex.h>
#include <Timestamp.h>
#include <Condition.h>
#include <Poller.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fas {

class Events;
class Handle;
class MutexLocker;
class TimersScheduler;
class Timer;

class EventLoop : boost::noncopyable {
public:
    typedef boost::shared_ptr<Handle> SHandlePtr;
    typedef Handle* HandlePtr;
    typedef boost::function<void ()> Functor;

    EventLoop(Poller::type polltype = Poller::type::EPOLL);
    ~EventLoop();

    long getTid() const;
	void resetOwnerTid();

    int getEventLoopNum() const;
    
    bool addHandle(HandlePtr handle);
    bool addSHandle(SHandlePtr handle);
    bool modHandle(HandlePtr handle);
    bool modSHandle(SHandlePtr handle);
    bool delHandle(HandlePtr handle);
    bool delSHandle(SHandlePtr handle);
    
    void addTimer(Timer *timer);
    void delTimer(Timer *timer);

    bool isInLoopOwnerThread();
    void assertInOwnerThread();  

    void runInLoop(const Functor& cb);
    void queueInLoop(const Functor& cb);

    void quit();
    void wakeUp();
    bool loop();

private:
    bool updateHandle(SHandlePtr handle);
    bool updateHandles();
    void runFunctors();

    void handWakeUp(Events event, fas::Timestamp time);

    static const int kInitMaxHandle_ = 10;
    static std::atomic<int> count_;
    Poller *poll_;
    int pollDelayTime_;
    std::vector<Events> revents_;
    std::map<int, SHandlePtr> handles_;
    std::map<int, SHandlePtr> updates_;
    Mutex mutex_;
    Condition cond_;
    long tid_;

    int wakeUpFd_;
    boost::shared_ptr<Handle> wakeUpHandle_;  /*!< The handle of wakeUpFd_. */

    std::vector<Functor> functors_;
    bool runningFunctors_;              /*!< Judge functor in functors_ was executing. */

    boost::scoped_ptr<TimersScheduler> timerScheduler_;   /*!< Timer's Scheduler */

    bool quit_;
};
/*!
 * \brief createEventfd
 * \return eventfd(int)
 * Create a eventfd NONBLOCK and CLOEXEC.
 */
int createEventfd();

}
#endif // FAS_EVENTLOOP_H
