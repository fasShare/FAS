#ifndef FAS_MUTEX_H
#define FAS_MUTEX_H
#include <pthread.h>

/*!
 * \brief The Mutex class
 * Encapsulation of pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy
 */
class Mutex {
private:
    pthread_mutex_t mutex_;
public:
    Mutex();
    void lock();
    void unlock();
    pthread_mutex_t* getMutex();
    ~Mutex();
};

#endif // FAS_MUTEX_H
