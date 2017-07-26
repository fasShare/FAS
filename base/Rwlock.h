#ifndef FAS_RWLOCK_H
#define FAS_RWLOCK_H
#include <pthread.h>

namespace fas {

class Rwlock {
private:
    pthread_rwlock_t lock_;
public:
    Rwlock();
    bool rlock();
    bool tryrlock();
    bool trywlock();
    bool wlock(); 
    bool unlock();
    ~Rwlock();
};

}
#endif // FAS_RWLOCK_H
