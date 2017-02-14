#ifndef FAS_DISPATCHER_H
#define FAS_DISPATCHER_H
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
#include <Types.h>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

class Handle;
using namespace std;

class Dispatcher : boost::noncopyable {
public:

  typedef shared_ptr<Handle> SHandlePtr;
  typedef Handle* HandlePtr;

  Dispatcher();
  ~Dispatcher();

  bool addHandle(HandlePtr handle);
  bool modHandle(HandlePtr handle);
  bool delHandle(HandlePtr handle);
  bool updateHandle(HandlePtr handle);
  void assertInDispatch();
  void dispatch();
private:
  bool updateHandle(SHandlePtr handle);
  bool delHandle(SHandlePtr handle);
  bool modHandle(SHandlePtr handle);
  bool addHandle(SHandlePtr handle);
  SHandlePtr handleSharedPtr(int fd);
  SHandlePtr handleSharedPtr(HandlePtr handle);
private:
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

#endif // FAS_DISPATCHER_H
