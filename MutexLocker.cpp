#include "MutexLocker.h"

MutexLocker::MutexLocker(Mutex& mutex):mMutex(mutex){
    mMutex.lock();
}

MutexLocker::~MutexLocker(){
    mMutex.unlock();
}
