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

class Handle;
class Epoll;

using namespace std;
class Dispatcher
{
private:
  static const int init_max_handle = 10;
  Mutex mutex;
  Poller *poll;
  vector<Events> revents;
  map<int, shared_ptr<Handle>> handles;
  map<int, shared_ptr<Handle>> update_handles;



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
