#include <Condition.h>
#include <errno.h>

Condition::Condition(Mutex& mutex) :
    mutex_(mutex) {
  pthread_cond_init(&cond_, NULL);
}

Condition::~Condition() {
  pthread_cond_destroy(&cond_);
}

void Condition::wait() {
  pthread_cond_wait(&cond_, mutex_.getMutex());
}

// returns true if time out, false otherwise.
bool Condition::waitForSeconds(int seconds) {
  struct timespec abstime;
  clock_gettime(CLOCK_REALTIME, &abstime);
  abstime.tv_sec += seconds;
  return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getMutex(), &abstime);
}

void Condition::notify() {
  pthread_cond_signal(&cond_);
}

void Condition::notifyAll() {
  pthread_cond_broadcast(&cond_);
}



