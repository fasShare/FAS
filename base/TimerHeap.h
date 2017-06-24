#ifndef FAS_TIMEHEAP_H
#define FAS_TIMEHEAP_H
#include <set>
#include <utility>
#include <vector>
#include <Timestamp.h>


#include <boost/shared_ptr.hpp>

namespace fas {

class Timer;
/*!
 * \brief The TimerHeap class
 * The TimerHeap class Order the Timer by Timer's expired time in less oeder.
 */
class TimerHeap {
public:
    typedef int timerfd_t;
    typedef boost::shared_ptr<Timer> TimerPtr;
    typedef std::set<std::pair<Timestamp, Timer*>>::iterator TimerHeapIter;
    typedef std::pair<TimerHeapIter, bool> TimerHeapSetRetType;
    TimerHeap();
    ~TimerHeap();

    uint getTimerNum() const;

    /*!
     * \brief getEarlistChange
     * \return bool
     * \see addTimer, delTimer
     * if addTimer and delTimer cause the earlist expired Timer change,
     * this funtion will return true, else return false.
     */
    bool getEarlistChange() const;
    Timestamp getEarlistExpiration() const;
    void getExpiredTimers(std::vector<std::pair<Timestamp, Timer *>>& expired, Timestamp now);

    void restartIntervalTimer(std::vector<std::pair<Timestamp, Timer *>>& expired);
    /*!
     * \brief addTimer
     * \param timer
     * \return bool
     * \see delTimer
     * add  timer to TimerHeap. if successful return true.
     * timer must be created by new Timer().
     */
    bool addTimer(Timer *timer);
    /*!
     * \brief delTimer
     * \param timer
     * delete timer from TimerHeap. timer must be created by new Timer().
     * timer's memory will be auto released.
     */
    void delTimer(Timer *timer);
private:
    std::set<std::pair<Timestamp, Timer *>> timers_;
    Timer *earlistExpiration_;
    bool earlistChange_;  /*!< The flag of earlist expired Timer change. */
};

}
#endif // FAS_TIMEHEAP_H

