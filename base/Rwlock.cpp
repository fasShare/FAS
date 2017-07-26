#include <errno.h>

#include "Rwlock.h"

fas::Rwlock::Rwlock() {
    lock_ = PTHREAD_RWLOCK_INITIALIZER;
}

fas::Rwlock::~Rwlock(){
    ::pthread_rwlock_destroy(&lock_);
}

bool fas::Rwlock::rlock() {
    int ret = ::pthread_rwlock_rdlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool fas::Rwlock::tryrlock() {
    int ret = ::pthread_rwlock_tryrdlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool fas::Rwlock::trywlock() {
    int ret = ::pthread_rwlock_wrlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool fas::Rwlock::wlock() {
    int ret = ::pthread_rwlock_wrlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool fas::Rwlock::unlock() {
    int ret = ::pthread_rwlock_unlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}



