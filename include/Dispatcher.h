#ifndef FAS_DISPATCHER_H
#define FAS_DISPATCHER_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <Events.h>
#include <Mutex.h>
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

//Dispatcher list operator!
	Dispatcher* next();
	Dispatcher* pre();
	static Dispatcher* head();
  static int count();
	int id();

	Dispatcher* nextLoop();

	bool addHandle(SHandlePtr handle);
  bool addHandle(HandlePtr handle);
  bool modHandle(SHandlePtr handle);
  bool modHandle(HandlePtr handle);
  bool delHandle(SHandlePtr handle);
  bool delHandle(HandlePtr handle);

  bool updateHandle(HandlePtr handle);

  SHandlePtr handleSharedPtr(int fd);
  SHandlePtr handleSharedPtr(HandlePtr handle);

  void loop();

private:
  bool updateHandle(SHandlePtr handle);

private:
  static const int kInitMaxHandle_ = 10;
  Mutex mutex_;
  boost::scoped_ptr<Poller> poll_;
  vector<Events> revents_;
  map<int, SHandlePtr> handles_;
  map<int, SHandlePtr> updates_;

//create Dispatcher list auto!
	static int count_;
  int id_;
	static Dispatcher *last_;
  static Dispatcher *head_;
	static Dispatcher *cur_;
  Dispatcher *next_;
	Dispatcher *pre_;
};

#endif // FAS_DISPATCHER_H
