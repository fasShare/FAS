#include "Mutex.h"

fas::Mutex::Mutex() {
  mutex_ = PTHREAD_MUTEX_INITIALIZER;
}
fas::Mutex::~Mutex(){
  ::pthread_mutex_destroy(&mutex_);
}

void fas::Mutex::lock(){
  ::pthread_mutex_lock(&mutex_);
}

void fas::Mutex::unlock(){
  ::pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* fas::Mutex::getMutex() {
  return &mutex_;
}


