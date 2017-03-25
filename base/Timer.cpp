#include <Timer.h>
#include <Log.h>

fas::Timer::Timer(const fas::TimerCallback& cb, fas::Timestamp when, double interval) :
  callback_(cb),
  expiration_(when),
  interval_(interval),
  repeat_(interval > 0.0),
  state_(Timer::STATE::ADDED) {
}

void fas::Timer::run() const {
  callback_();
}

fas::Timestamp fas::Timer::getExpiration() const {
  return expiration_;
}

bool fas::Timer::getRepeat() const {
  return repeat_;
}

fas::Timer::STATE fas::Timer::getState() const {
  return state_;
}

void fas::Timer::setState(Timer::STATE state) {
  state_ = state;
}

void fas::Timer::restart(fas::Timestamp now) {
  if (repeat_) {
    expiration_ = fas::addTime(now, interval_);
  }
  else {
    expiration_ = fas::Timestamp::invalid();
  }
}

fas::Timer::~Timer() {
  LOGGER_TRACE << "Destroy one Timer!" << fas::Log::CLRF;
}
