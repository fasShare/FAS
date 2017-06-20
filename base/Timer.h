#ifndef FAS_TIMER_H
#define FAS_TIMER_H
#include <Timestamp.h>


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fas {

typedef boost::function<void ()> TimerCallback;
/*!
 * \brief The Timer class
 */
class Timer : boost::noncopyable {
public:
    enum STATE {
        ADDED,
        DELETED,
    };
    Timer(const TimerCallback& cb, Timestamp when, double interval);
    ~Timer();
    void run() const;
    /*!
     * \brief getExpiration
     * \return Timestamp
     * The value of return is param(when) of Timer ctor.
     */
    Timestamp getExpiration() const;
    /*!
     * \brief getRepeat
     * \return bool
     * if getRepeat() == true, Timer will not be auto deleted,
     * although the Timer is expired, if Timer expired, it's restart() will be called.
     * you can call TimerHeap's delTimer delete it.
     */
    bool getRepeat() const;

    Timer::STATE getState() const;
    void setState(Timer::STATE state);
    /*!
     * \brief restart
     * \param now
     * If getRepeat == true this function will be called by TimerHeap when this Timer is expired.
     * To update this Timer's new Expiration Timestamp. if this Timer was deleted, restart will not be called,
     * although getRepeat == true.
     */
    void restart(Timestamp now);
private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;        /*!< The interval of Timer expired. */
    const bool repeat_;            /*!< if repeat_ == true, Timer will not be auto deleted.
                                     you can call TimerHeap's delTimer delete it. */
    Timer::STATE state_;           /*!< value between STATE::ADDED and STATE::DELETED. */
};

}
#endif  // FAS_TIMER_H
