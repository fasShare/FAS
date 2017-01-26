#ifndef FAS_TIMESTAMP_H
#define FAS_TIMESTAMP_H
#include <string>

using std::string;

class Timestamp {
 public:
  ///
  /// Timestamp can not used
  ///
  Timestamp()
    : microSecondsSinceEpoch_(0)
  {
  }

  ///
  /// Constucts a Timestamp at specific time
  ///
  /// @param microSecondsSinceEpoch
  explicit Timestamp(int64_t microSecondsSinceEpoch);

  void swap(Timestamp& that);

  // default copy/assignment/dtor are Okay

  string toString() const;
  string toFormattedString() const;

  bool isvalid() const;

  // for internal usage.
  int64_t get_microSecondsSinceEpoch() const;
  time_t secondsSinceEpoch() const;
  ///
  /// Get time of now.
  ///
  static Timestamp now();

  static const int kMicroSecondsPerSecond = 1000 * 1000;

 private:
  int64_t microSecondsSinceEpoch_;
};


bool operator<(Timestamp lhs, Timestamp rhs);
bool operator>(Timestamp lhs, Timestamp rhs);
bool operator==(Timestamp lhs, Timestamp rhs);
double timeDifference(Timestamp high, Timestamp low);
Timestamp addTime(Timestamp timestamp, double seconds);

#endif  // FAS_TIMESTAMP_H
