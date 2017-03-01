#ifndef FAS_MUTEXLOCKER_H
#define FAS_MUTEXLOCKER_H
#include <pthread.h>


class Mutex;

/*!
 * \brief The MutexLocker class
 * A local manager of Mutex class
 */
class MutexLocker {
public:
  MutexLocker(Mutex& mutex);
  ~MutexLocker();
private:
  Mutex& mutex_;
};

#endif // FAS_MUTEXLOCKER_H
