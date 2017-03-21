#include <MutexLocker.h>
#include <Mutex.h>

using fas::MutexLocker;

MutexLocker::MutexLocker(Mutex& mutex):mutex_(mutex){
    mutex_.lock();
}

MutexLocker::~MutexLocker(){
    mutex_.unlock();
}
