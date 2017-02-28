#ifndef FAS_TIMER_H
#define FAS_TIMER_H
#include <Timestamp.h>


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

typedef boost::function<void ()> TimerCallback;

class Timer : boost::noncopyable
{
 public:
  enum STATE {
    ADDED,
    DELETED,
  };
  Timer(const TimerCallback& cb, Timestamp when, double interval);
  ~Timer();
  void run() const;

  Timestamp getExpiration() const;
  bool getRepeat() const;

  Timer::STATE getState() const;
  void setState(Timer::STATE state);

  void restart(Timestamp now);
 private:
  const TimerCallback callback_;
  Timestamp expiration_;
  const double interval_;
  const bool repeat_;
  Timer::STATE state_;
};

#endif  // FAS_TIMER_H
