#ifndef FAS_EVENTLOOP_H
#define FAS_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <Events.h>
#include <Mutex.h>
#include <Condition.h>
#include <MutexLocker.h>
#include <Handle.h>
#include <Poller.h>
#include <Default.h>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

class Handle;
using namespace std;

class EventLoop : boost::noncopyable {
public:

  typedef shared_ptr<Handle> SHandlePtr;
  typedef Handle* HandlePtr;

  EventLoop();
  ~EventLoop();

  bool addHandle(HandlePtr handle);
  bool modHandle(HandlePtr handle);
  bool delHandle(HandlePtr handle);

  void assertInOwner();

  void loop();
private:
  bool updateHandle(HandlePtr handle);
  bool addNewToHandles();

  static const int kInitMaxHandle_ = 10;
  static int count_;
  boost::scoped_ptr<Poller> poll_;
  vector<Events> revents_;
  map<int, SHandlePtr> handles_;
  map<int, SHandlePtr> updates_;
  Mutex mutex_;
  Condition cond_;
  pid_t tid_;
};

#endif // FAS_EVENTLOOP_H
