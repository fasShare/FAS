#include <vector>
#include <utility>
#include <algorithm>


#include <TimersScheduler.h>
#include <EventLoop.h>
#include <Handle.h>
#include <Events.h>
#include <Timer.h>
#include <Timestamp.h>
#include <Log.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

TimersScheduler::TimersScheduler(EventLoop *loop) :
  timerfd_(::timerfd_create(CLOCK_MONOTONIC, 0)),
  loop_(loop),
  handle_(new Handle(loop_, Events(timerfd_, kReadEvent))),
  timerheap_(),
  expired_(),
  timerCallbackRunning_(false) {

  if (timerfd_ ==  -1) {
    LOGGER_SYSERR << "timerfd_create error : " << ::strerror(errno) << Log::CLRF;
  }

  assert(handle_->getLoop() == loop_);

  //set handle callbacks
  handle_->setHandleRead(boost::bind(&TimersScheduler::handdleRead, this, _1, _2));

  loop_->addHandle(handle_);
}

timerfd_t TimersScheduler::getTimerfd() const {
  return timerfd_;
}

struct itimerspec TimersScheduler::calculateTimerfdNewValue(Timestamp earlist) {
  int64_t microseconds =  earlist.get_microSecondsSinceEpoch() - \
                            Timestamp::now().get_microSecondsSinceEpoch();
  if (microseconds < 100) {
    microseconds = 100;
  }
/*!
 * The it_interval field returns the interval of the timer.
 * If both fields of this structure are zero,
 * then the timer is set to expire just once,
 * at the time specified by curr_value.it_value.
 */
  struct itimerspec newvalue;
  newvalue.it_interval.tv_sec = 0;
  newvalue.it_interval.tv_nsec = 0;

  newvalue.it_value.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
  newvalue.it_value.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);

  return newvalue;
}

bool TimersScheduler::resetTimer(Timestamp earlist) {
  struct itimerspec newvalue;
  struct itimerspec oldvalue;
  bzero(&newvalue, sizeof(struct itimerspec));
  bzero(&oldvalue, sizeof(struct itimerspec));
  newvalue = calculateTimerfdNewValue(earlist);
  int ret = ::timerfd_settime(timerfd_, 0, &newvalue, &oldvalue);
  if (ret < 0) {
    LOGGER_SYSERR << "timerfd_settime error : " << ::strerror(errno) << Log::CLRF;
    return false;
  }
  return true;
}
// If Timer add itself in it's callback. this operator will failed.
bool TimersScheduler::addTimer(Timer *timer) {
  loop_->assertInOwnerThread();
  if (timerCallbackRunning_ == true) {
    auto iter = std::find(expired_.begin(), expired_.end(), \
                          std::make_pair(timer->getExpiration(), timer));
    if (iter != expired_.end()) {
      return false;
    }
  }
  timerheap_.addTimer(timer);
  if(timerheap_.getEarlistChange()) {
    Timestamp earlist = timerheap_.getEarlistExpiration();
    return resetTimer(earlist);
  }
  return true;
}

void TimersScheduler::delTimer(Timer *timer) {
  loop_->assertInOwnerThread();
  if (timerCallbackRunning_ == true) {
    auto iter = std::find(expired_.begin(), expired_.end(), \
                          std::make_pair(timer->getExpiration(), timer));
    if (iter != expired_.end()) {
      iter->second->setState(Timer::STATE::DELETED);
      return;
    }
  }
  timerheap_.delTimer(timer);
  if (timerheap_.getEarlistChange()) {
    Timestamp earlist = timerheap_.getEarlistExpiration();
    resetTimer(earlist);
  }
}

void TimersScheduler::handdleRead(Events events, Timestamp time) {
  loop_->assertInOwnerThread();
  boost::ignore_unused(events, time);
  // now is more accurate than the time of loop wait returned.
  Timestamp now = Timestamp::now();
  timerheap_.getExpiredTimers(expired_, now);
  timerCallbackRunning_ = true;
  for (auto iter = expired_.begin(); iter != expired_.end(); iter++) {
    if (iter->second->getState() != Timer::STATE::DELETED) {
      iter->second->run();
    }
  }
  timerCallbackRunning_ = false;
  timerheap_.restartIntervalTimer(expired_);
  if (timerheap_.getEarlistChange()) {
    resetTimer(timerheap_.getEarlistExpiration());
  }
  expired_.clear();
}

TimersScheduler::~TimersScheduler() {
  for (auto iter = expired_.begin(); iter != expired_.end(); ++iter) {
    if (iter->second != nullptr) {
      delete iter->second;
      iter->second = nullptr;
    }
  }
}
