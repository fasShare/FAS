#include <errno.h>

#include <Condition.h>
#include <Mutex.h>

fas::Condition::Condition(Mutex& mutex) :
    mutex_(mutex) {
    ::pthread_cond_init(&cond_, NULL);
}

fas::Condition::~Condition() {
    ::pthread_cond_destroy(&cond_);
}

void fas::Condition::wait() {
    ::pthread_cond_wait(&cond_, mutex_.getMutex());
}

bool fas::Condition::waitForSeconds(int seconds) {
    struct timespec abstime;
    ::clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += seconds;
    return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getMutex(), &abstime);
}

void fas::Condition::notify() {
    ::pthread_cond_signal(&cond_);
}

void fas::Condition::notifyAll() {
    ::pthread_cond_broadcast(&cond_);
}



