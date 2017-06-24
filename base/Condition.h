#ifndef FAS_CONDITION_H
#define FAS_CONDITION_H
#include <pthread.h>


#include <boost/noncopyable.hpp>

namespace fas {

class Mutex;
/*!
 * \brief The Condition class
 * Encapsulation of pthread_cond_wait, pthread_cond_signal, pthread_cond_broadcast
 */
class Condition : boost::noncopyable {
public:
    Condition(Mutex& mutex);
    ~Condition() ;

    void wait();
    // returns true if time out, false otherwise.
    bool waitForSeconds(int seconds);

    void notify() ;
    void notifyAll();

private:
    Mutex& mutex_;
    pthread_cond_t cond_;
};

}
#endif  // FAS_CONDITION_H
