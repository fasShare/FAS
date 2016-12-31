#ifndef MUTEXLOCKER_H
#define MUTEXLOCKER_H
#include <pthread.h>
#include "Mutex.h"
class MutexLocker
{
private:
    Mutex& mMutex;
public:
    MutexLocker(Mutex& mutex);
    ~MutexLocker();
};

#endif // MUTEXLOCKER_H
