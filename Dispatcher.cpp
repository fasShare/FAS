#include "Dispatcher.h"
#include <assert.h>

Dispatcher::Dispatcher():
                        poll(NULL){
  revents.reserve(init_max_events);
  poll = EventsPoller::Poller_create();
  assert(poll);
}

bool Dispatcher::Dispatcher_add_events(shared_ptr<Executor> exec) {
  MutexLocker lock(mutex);
  (void)lock;
  add_execs.insert({exec->Executor_get_event().Events_get_fd(),\
                    exec});
  return true;
}

bool Dispatcher::Dispatcher_add_events(Executor* exec) {
  shared_ptr<Executor> exec_ptr(exec);
  return Dispatcher_add_events(exec_ptr);
}

bool Dispatcher::Dispatcher_mod_events(shared_ptr<Executor> exec) {
  return Dispatcher_add_events(exec);
}

bool Dispatcher::Dispatcher_mod_events(Executor* exec) {
  return Dispatcher_add_events(exec);
}

bool Dispatcher::Dispatcher_del_events(shared_ptr<Executor> exec) {
  MutexLocker lock(mutex);
  (void)lock;
  del_execs.insert({exec->Executor_get_event().Events_get_fd(),\
                    exec});
  return true;
}

bool Dispatcher::Dispatcher_del_events(Executor* exec) {
  shared_ptr<Executor> exec_ptr(exec);
  return Dispatcher_add_events(exec_ptr);
}

void Dispatcher::Dispatcher_loop() {
  while (true) {

    for(map<int, shared_ptr<Executor>>::iterator index = add_execs.begin(); \
        index != add_execs.end(); index++) {

      assert(index->first == index->second->Executor_get_eventpointer()->Events_get_fd());
      //insert or update
      if (execs.find(index->first) == execs.end()) {
        poll->Poller_event_add((*index).second->Executor_get_eventpointer());
        execs.insert({index->first,\
                     index->second});
      }else{
        poll->Poller_event_mod((*index).second->Executor_get_eventpointer());
        execs[index->first] = index->second;
      }
    }
    add_execs.clear();


    int ret = poll->Poller_loop(revents, execs.size(), -1);

    for(int i = 0; i < ret; i++) {
      int fd = revents.data()[i].Events_get_fd();
      //exec will decreament reference after for end!
      shared_ptr<Executor> exec = execs.find(fd)->second;
      assert(exec->Executor_get_eventpointer()->Events_get_fd() == fd);
      exec->Executor_handle_event(&(revents.data()[i]));
    }
  }
}

