#include <Timer.h>
#include <Log.h>

using fas::Timer;

Timer::Timer(const fas::TimerCallback& cb, fas::Timestamp when, double interval) :
  callback_(cb),
  expiration_(when),
  interval_(interval),
  repeat_(interval > 0.0),
  state_(Timer::STATE::ADDED) {
}

void Timer::run() const {
  callback_();
}

fas::Timestamp Timer::getExpiration() const {
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

void Timer::restart(fas::Timestamp now) {
  if (repeat_) {
    expiration_ = fas::addTime(now, interval_);
  }
  else {
    expiration_ = fas::Timestamp::invalid();
  }
}

Timer::~Timer() {
  LOGGER_TRACE << "Destroy one Timer!" << fas::Log::CLRF;
}
