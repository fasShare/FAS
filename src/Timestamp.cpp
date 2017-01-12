#include "Timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS


Timestamp::Timestamp(int64_t microseconds) :
    microSecondsSinceEpoch_(microseconds) {
}

string Timestamp::toString() const {
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString() const {
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

bool Timestamp::isvalid() const {
  return microSecondsSinceEpoch_ > 0;
}

void Timestamp::swap(Timestamp& that) {
  std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
}

int64_t Timestamp::get_microSecondsSinceEpoch() const {
  return microSecondsSinceEpoch_;
}

time_t Timestamp::secondsSinceEpoch() const {
  return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
}

bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.get_microSecondsSinceEpoch() < rhs.get_microSecondsSinceEpoch();
}

bool operator>(Timestamp lhs, Timestamp rhs) {
  return lhs.get_microSecondsSinceEpoch() > rhs.get_microSecondsSinceEpoch();
}

bool operator==(Timestamp lhs, Timestamp rhs) {
  return lhs.get_microSecondsSinceEpoch() == rhs.get_microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microseciond
/// resolution for next 100 years.
double timeDifference(Timestamp high, Timestamp low) {
  int64_t diff = high.get_microSecondsSinceEpoch() - low.get_microSecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
Timestamp addTime(Timestamp timestamp, double seconds) {
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.get_microSecondsSinceEpoch() + delta);
}
