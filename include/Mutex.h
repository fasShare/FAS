#ifndef MUTEX_H
#define MUTEX_H
#include <pthread.h>

class Mutex
{
private:
    pthread_mutex_t mutex_;
public:
    Mutex();
    void lock();
    void unlock();
    ~Mutex();
};

#endif // MUTEX_H
