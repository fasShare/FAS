#ifndef FAS_TIMERSSCHEDULER_H
#define FAS_TIMERSSCHEDULER_H
#include <sys/timerfd.h>


#include <Types.h>
#include <TimerHeap.h>
#include <Timestamp.h>

class Handle;
class EventLoop;

class TimersScheduler {
public:
  TimersScheduler(EventLoop *loop);

  timerfd_t getTimerfd() const;

  bool addTimer(Timer *);
  void delTimer(Timer *);

  void handdleRead(Events events, Timestamp time);

private:
  struct itimerspec calculateTimerfdNewValue(Timestamp earlist);
  bool resetTimer(Timestamp earlist);

  timerfd_t timerfd_;
  EventLoop *loop_;
  Handle *handle_;
  TimerHeap timerheap_;

  std::vector<std::pair<Timestamp, Timer*>> expired_;

  bool timerCallbackRunning_;
};

#endif // TIMERSSCHEDULER_H

