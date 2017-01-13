#ifndef DISPATCHER_H
#define DISPATCHER_H
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
private:
  static const int kInitMaxHandle_ = 10;
  Mutex mutex_;
  boost::scoped_ptr<Poller> poll_;
  vector<Events> revents_;
  map<int, shared_ptr<Handle>> handles_;
  map<int, shared_ptr<Handle>> update_handles_;

public:
  Dispatcher();
  bool addHandle(shared_ptr<Handle> handle);
  bool addHandle(Handle* handle);
  bool modHandle(shared_ptr<Handle> handle);
  bool modHandle(Handle* handle);
  bool delHandle(shared_ptr<Handle> handle);
  bool delHandle(Handle* handle);

  shared_ptr<Handle>  get_handle_shared_ptr(int fd);
  shared_ptr<Handle>  get_handle_shared_ptr(Handle *handle);

  void loop();

private:
  bool updateHandle(shared_ptr<Handle> handle);
};

#endif // DISPATCHER_H
