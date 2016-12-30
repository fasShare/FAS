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
#include "Executor.h"
#include "NetBaseTypes.h"

class Executor;
class Epoll;

using namespace std;
class Dispatcher
{
private:
  static const int init_max_events = 10;
  Mutex mutex;
  EventsPoller *poll;
  vector<Events> revents;
  map<int, shared_ptr<Executor>> execs;
  map<int, shared_ptr<Executor>> add_execs;
  map<int, shared_ptr<Executor>> del_execs;
public:
  Dispatcher();
  bool Dispatcher_add_events(shared_ptr<Executor> exec);
  bool Dispatcher_add_events(Executor* exec);
  bool Dispatcher_mod_events(shared_ptr<Executor> exec);
  bool Dispatcher_mod_events(Executor* exec);
  bool Dispatcher_del_events(shared_ptr<Executor> exec);
  bool Dispatcher_del_events(Executor* exec);

  void Dispatcher_loop();
};

#endif // DISPATCHER_H
