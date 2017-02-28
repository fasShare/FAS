#ifndef FAS_TIMEHEAP_H
#define FAS_TIMEHEAP_H
#include <set>
#include <utility>
#include <vector>


#include <Types.h>

class Timer;

class TimerHeap {
public:
  typedef std::set<std::pair<Timestamp, Timer*>>::iterator TimerHeapIter;
  typedef std::pair<TimerHeapIter, bool> TimerHeapSetRetType;
  TimerHeap();

  uint getTimerNum() const;
  bool getEarlistChange() const;
  Timestamp getEarlistExpiration() const;
  void getExpiredTimers(std::vector<std::pair<Timestamp, Timer *>>& expired, Timestamp now);

  void restartIntervalTimer(std::vector<std::pair<Timestamp, Timer *>>& expired);

  bool addTimer(Timer *timer);
  void delTimer(Timer *timer);
private:
  std::set<std::pair<Timestamp, Timer *>> timers_;
  Timer *earlistExpiration_;
  bool earlistChange_;
};

#endif // FAS_TIMEHEAP_H

