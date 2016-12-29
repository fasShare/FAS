#include "Mutex.h"

Mutex::Mutex()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
}
Mutex::~Mutex(){
    pthread_mutex_destroy(&mutex);
}

void Mutex::lock(){
    pthread_mutex_lock(&mutex);
}

void Mutex::unlock(){
    pthread_mutex_unlock(&mutex);
}



