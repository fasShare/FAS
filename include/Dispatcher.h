#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "Epoll.h"
#include "Events.h"
#include "Mutex.h"
#include "MutexLocker.h"
#include "Handle.h"
#include "NetBaseTypes.h"

#include <boost/scoped_ptr.hpp>

class Handle;

using namespace std;
class Dispatcher
{
private:
  static const int kInitMaxHandle_ = 10;
  Mutex mutex_;
  boost::scoped_ptr<Poller> poll_;
  vector<Events> revents_;
  map<int, shared_ptr<Handle>> handles_;
  map<int, shared_ptr<Handle>> update_handles_;



public:
  Dispatcher();
  bool add_handle(shared_ptr<Handle> handle);
  bool add_handle(Handle* handle);
  bool mod_handle(shared_ptr<Handle> handle);
  bool mod_handle(Handle* handle);
  bool del_handle(shared_ptr<Handle> handle);
  bool del_handle(Handle* handle);

  shared_ptr<Handle>  get_handle_shared_ptr(int fd);
  shared_ptr<Handle>  get_handle_shared_ptr(Handle *handle);

  void loop();

private:
  bool update_handle(shared_ptr<Handle> handle);
};

#endif // DISPATCHER_H
