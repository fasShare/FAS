#ifndef FAS_EVENTLOOP_H
#define FAS_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include <Types.h>
#include <Mutex.h>
#include <Condition.h>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

class Events;
class Handle;
class MutexLocker;
class Poller;
class TimersScheduler;
class timer;

class EventLoop : boost::noncopyable {
public:

  typedef std::shared_ptr<Handle> SHandlePtr;
  typedef Handle* HandlePtr;
  typedef boost::function<void ()> Functor;

  EventLoop();
  ~EventLoop();

  int getCount() const;

  bool addHandle(HandlePtr handle);
  bool modHandle(HandlePtr handle);
  bool delHandle(HandlePtr handle);

  void addTimer(Timer *timer);
  void delTimer(Timer *timer);

  bool isInLoopOwnerThread();
  //判断当前loop是否在拥有它的线程中。
  void assertInOwnerThread();  

  /// Runs callback immediately in the loop thread.
  /// It wakes up the loop, and run the cb.
  /// If in the same loop thread, cb is run within the function.
  /// Safe to call from other threads.
  void runInLoop(const Functor& cb);
  /// Queues callback in the loop thread.
  /// Runs after finish pooling.
  /// Safe to call from other threads.
  void queueInLoop(const Functor& cb);

  void quit();

  void wakeUp();

  bool loop();
private:
  bool updateHandle(SHandlePtr handle);
  bool updateHandles();
  void runFunctors();

  void handWakeUp(Events event, Timestamp time);

  static const int kInitMaxHandle_ = 10;
  static int count_;
  boost::scoped_ptr<Poller> poll_;
  int pollDelayTime_;
  std::vector<Events> revents_;
  std::map<int, SHandlePtr> handles_;
  std::map<int, SHandlePtr> updates_;
  Mutex mutex_;
  Condition cond_;
  pid_t tid_;

  //从一个loop向另外一个loop传递handle时，用来把线程从loop循环中唤醒。
  int wakeUpFd_;
  Handle *wakeUpHandle_;

  std::vector<Functor> functors_;
  bool runningFunctors_;

  TimersScheduler *timerScheduler_;

  bool quit_;
};

int createEventfd();

#endif // FAS_EVENTLOOP_H
