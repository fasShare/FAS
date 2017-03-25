#include <MutexLocker.h>
#include <Mutex.h>

fas::MutexLocker::MutexLocker(fas::Mutex& mutex):mutex_(mutex){
    mutex_.lock();
}

fas::MutexLocker::~MutexLocker(){
    mutex_.unlock();
}
