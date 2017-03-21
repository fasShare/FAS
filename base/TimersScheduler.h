#ifndef FAS_TIMERSSCHEDULER_H
#define FAS_TIMERSSCHEDULER_H
#include <sys/timerfd.h>


#include <Types.h>
#include <TimerHeap.h>
#include <Timestamp.h>

namespace fas {

class Handle;
class EventLoop;

/*!
 * \brief The TimersScheduler class
 * The TimersScheduler class create a timerfd, and set the earlist expired time
 * to this timerfd, bind timerfd and Events on handle_,
 * when the expired time timeout, the TimersScheduler::handdleRead
 * will be call, in this handdleRead function we can find out all expired Timer and call
 * Timers's callback, if Timer is repeat, we'll restart the Timer.
 */
class TimersScheduler {
public:
  TimersScheduler(EventLoop *loop);
  ~TimersScheduler();

  timerfd_t getTimerfd() const;

  bool addTimer(Timer *);
  void delTimer(Timer *);

  void handdleRead(Events events, Timestamp time);

private:
  struct itimerspec calculateTimerfdNewValue(Timestamp earlist);
  /*!
   * \brief resetTimer
   * \param earlist
   * \return bool
   * if addTimer or delTimer cause the earlist expired time changed,
   * we can call resetTimer retset the readable Time of timerfd_.
   */
  bool resetTimer(Timestamp earlist);

  timerfd_t timerfd_;
  EventLoop *loop_;
  Handle *handle_;
  TimerHeap timerheap_;

  std::vector<std::pair<Timestamp, Timer*>> expired_;

  bool timerCallbackRunning_;
};

}
#endif // TIMERSSCHEDULER_H

