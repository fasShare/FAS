#include "Mutex.h"

Mutex::Mutex() {
  mutex_ = PTHREAD_MUTEX_INITIALIZER;
}
Mutex::~Mutex(){
  pthread_mutex_destroy(&mutex_);
}

void Mutex::lock(){
  pthread_mutex_lock(&mutex_);
}

void Mutex::unlock(){
  pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* Mutex::getMutex() {
  return &mutex_;
}


