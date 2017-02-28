#include <Timer.h>
#include <Log.h>

Timer::Timer(const TimerCallback& cb, Timestamp when, double interval) :
  callback_(cb),
  expiration_(when),
  interval_(interval),
  repeat_(interval > 0.0),
  state_(Timer::STATE::ADDED) {
}

void Timer::run() const {
  callback_();
}

Timestamp Timer::getExpiration() const {
  return expiration_;
}

bool Timer::getRepeat() const {
  return repeat_;
}

Timer::STATE Timer::getState() const {
  return state_;
}

void Timer::setState(Timer::STATE state) {
  state_ = state;
}

void Timer::restart(Timestamp now) {
  if (repeat_) {
    expiration_ = addTime(now, interval_);
  }
  else {
    expiration_ = Timestamp::invalid();
  }
}

Timer::~Timer() {
  LOG_TRACE("Destroy one Timer!");
}
