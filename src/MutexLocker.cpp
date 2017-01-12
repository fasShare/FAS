#include "MutexLocker.h"

MutexLocker::MutexLocker(Mutex& mutex):mutex_(mutex){
    mutex_.lock();
}

MutexLocker::~MutexLocker(){
    mutex_.unlock();
}
