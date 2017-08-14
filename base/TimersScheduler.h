#ifndef FAS_TIMERSSCHEDULER_H
#define FAS_TIMERSSCHEDULER_H
#include <sys/timerfd.h>


#include <TimerHeap.h>
#include <Timestamp.h>
#include <Events.h>
#include <Handle.h>

namespace fas {

class TimersScheduler {
public:
    TimersScheduler(EventLoop *loop);
    ~TimersScheduler();

    TimerHeap::timerfd_t getTimerfd() const;

    bool addTimer(Timer *);
    void delTimer(Timer *);

    void handdleRead(Events events, Timestamp time);

private:
    struct itimerspec calculateTimerfdNewValue(Timestamp earlist);
    bool resetTimer(Timestamp earlist);

    TimerHeap::timerfd_t timerfd_;
    EventLoop *loop_;
    boost::shared_ptr<Handle> handle_;
    TimerHeap timerheap_;

    std::vector<std::pair<Timestamp, Timer*>> expired_;

    bool timerCallbackRunning_;
};

}
#endif // TIMERSSCHEDULER_H

