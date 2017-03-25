#ifndef FAS_EVENTLOOP_H
#define FAS_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <atomic>

#include <Types.h>
#include <Mutex.h>
#include <Condition.h>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fas {

class Events;
class Handle;
class MutexLocker;
class Poller;
class TimersScheduler;
class timer;
/*!
 * \brief The EventLoop class
 * The EventLoop class is core class of fas.
 * In EventLoop's loop function, call poller's loop_ function to
 * polling the events added to the file descriptor.
 * After the poller's loop_ function return, it call the comming events's callback in a for statement.
 * events's callback and Events was established relationship by handle.
 * We also can add timers to the EventLoop. timers was scheduled by the TimersScheduler class,
 * The TimersScheduler class establish the relationship between timerfd which can be added to poller and
 * Timers, so we call TimersScheduler's readcallback to execute our timer function.
 */
class EventLoop : boost::noncopyable {
public:

  typedef std::shared_ptr<Handle> SHandlePtr;
  typedef Handle* HandlePtr;
  typedef boost::function<void ()> Functor;

  EventLoop();
  ~EventLoop();

  /*!
   * \brief getCount
   * \return int
   * Get num of EventLoop was created by our program.
   */
  int getCount() const;
  /*!
   * \brief addHandle
   * \param handle
   * \return bool
   * \see modHandle, delHandle
   * Add a new handle to this EventLoop.
   * If the handle exist, this function will assert(false).
   */
  bool addHandle(HandlePtr handle);
  /*!
   * \brief modHandle
   * \param handle
   * \return bool
   * Modify the handle which was existting in this EventLoop.
   * If the handle not exist, this function will return false and no action.
   */
  bool modHandle(HandlePtr handle);
  /*!
   * \brief delHandle
   * \param handle
   * \return bool
   * Delete the handle which was existting in this EventLoop.
   * If the handle not exist, this function will return false and no action.
   */
  bool delHandle(HandlePtr handle);
  /*!
   * \brief addTimer
   * \param timer
   * Add a new Timer to this EventLoop.
   * If this Timer is exist, will failed.
   * The Timer must be created by new Timer().
   */
  void addTimer(Timer *timer);
  /*!
   * \brief delTimer
   * \param timer
   * Delete the Timer which was existting in this EventLoop.
   * The Timer's memory will be auto released.
   */
  void delTimer(Timer *timer);
  /*!
   * \brief isInLoopOwnerThread
   * \return bool
   * Judge this EventLoop whether in its own Thread.
   */
  bool isInLoopOwnerThread();
  void assertInOwnerThread();  

  /*!
   * \brief runInLoop
   * \param cb
   * Runs callback immediately in the loop thread.
   * It wakes up the loop, and run the cb.
   * If in the same loop thread, cb is run within the function.
   * Safe to call from other threads.
   */
  void runInLoop(const Functor& cb);

  /*!
   * \brief queueInLoop
   * \param cb
   * Queues callback in the loop thread.
   * Runs after finish pooling.
   * Safe to call from other threads.
   */
  void queueInLoop(const Functor& cb);

  /*!
   * \brief quit
   * Qiut the Eventloop's loop function. It's may not quit at once.
   */
  void quit();

  /*!
   * \brief wakeUp
   * If a handle or a functor was delivered to this Thread from other thread.
   * Other Thread can call this function to wake up this Thread from polling.
   */
  void wakeUp();
  /*!
   * \brief loop
   * \return bool
   * Polling events in this function.
   */
  bool loop();
private:
  bool updateHandle(SHandlePtr handle);
  bool updateHandles();
  void runFunctors();

  void handWakeUp(Events event, Timestamp time);

  static const int kInitMaxHandle_ = 10;   /*!< EventLoop's handle num can more than kInitMaxHandle_ */
  static std::atomic<int> count_;
  boost::scoped_ptr<Poller> poll_;         /*!< poller shared_ptr will be auto released. */
  int pollDelayTime_;
  std::vector<Events> revents_;            /*!< recv the revents was return by poller. */
  std::map<int, SHandlePtr> handles_;      /*!< Handles was added to this EventLoop. */
  std::map<int, SHandlePtr> updates_;      /*!< Handles will be addef to handles_. */
  Mutex mutex_;
  Condition cond_;
  pid_t tid_;                              /*!< The pid of the Thread which own this EventLoop. */

  int wakeUpFd_;          /*!< eventfd */
  Handle *wakeUpHandle_;  /*!< The handle of wakeUpFd_. */

  std::vector<Functor> functors_;
  bool runningFunctors_;              /*!< Judge functor in functors_ was executing. */

  TimersScheduler *timerScheduler_;   /*!< Timer's Scheduler */

  bool quit_;
};
/*!
 * \brief createEventfd
 * \return eventfd(int)
 * Create a eventfd NONBLOCK and CLOEXEC.
 */
int createEventfd();

}
#endif // FAS_EVENTLOOP_H
