#include "Dispatcher.h"
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

Dispatcher::Dispatcher():
                        poll(NULL){
  revents.reserve(init_max_events);
  poll = Poller::Poller_create();
  assert(poll);
}


bool Dispatcher::update_events(shared_ptr<Handle> handle) {
  MutexLocker lock(mutex);(void)lock;
  update_handles.insert({handle->get_event().get_fd(), handle});
  return true;
}


bool Dispatcher::add_events(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_ADD);
  return update_events(handle);
}
bool Dispatcher::add_events(Handle* handle) {
  shared_ptr<Handle> handle_ptr(handle);
  return add_events(handle_ptr);
}

//FIXME:mod by fd
bool Dispatcher::mod_events(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_MOD);
  return update_events(handle);
}
bool Dispatcher::mod_events(Handle* handle) {
  //shared_ptr<Handle> handle_ptr = handles.find(handle->get_event().get_fd())->second;
  shared_ptr<Handle> handle_ptr = get_handle_shared_ptr(handle);
  return mod_events(handle_ptr);
}

//FIXME:del by fd
bool Dispatcher::del_events(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_DEL);
  return update_events(handle);
}
bool Dispatcher::del_events(Handle* handle) {
  //shared_ptr<Handle> handle_ptr = handles.find(handle->get_event().get_fd())->second;
  shared_ptr<Handle> handle_ptr = get_handle_shared_ptr(handle);
  return del_events(handle_ptr);
}


shared_ptr<Handle>  Dispatcher::get_handle_shared_ptr(int fd) {
  map<int, shared_ptr<Handle>>::iterator index =  handles.find(fd);
  return index->second;
}

shared_ptr<Handle>  Dispatcher::get_handle_shared_ptr(Handle *handle) {
  map<int, shared_ptr<Handle>>::iterator index =  handles.find(handle->get_event().get_fd());
  return index->second;
}


void Dispatcher::loop() {
  while (true) {

    for(map<int, shared_ptr<Handle>>::iterator index = update_handles.begin(); \
                index != update_handles.end(); index++) {

      assert(index->first == \
             index->second->get_eventpointer()->get_fd());

      shared_ptr<Handle> op_handle = index->second;

      if (op_handle->get_state() == EXECUTOR_STATE_ADD) {

        poll->Poller_event_add(index->second->get_eventpointer());
        //insert
        handles[index->first] = index->second;
      } else if (op_handle->get_state() == EXECUTOR_STATE_MOD) {

        poll->Poller_event_mod(index->second->get_eventpointer());
        //mod
        handles[index->first] = index->second;
      } else if (op_handle->get_state() == EXECUTOR_STATE_DEL) {
        poll->Poller_event_del(op_handle->get_eventpointer());
        //del
        handles.erase(op_handle->get_event().get_fd());
      } else {
        //error
        assert(false);
      }
    }
    update_handles.clear();
    revents.clear();

    int ret = poll->Poller_loop(revents, 20, -1);

    for(int i = 0; i < ret; i++) { 
      int fd = revents.data()[i].get_fd();
     
      //handle will decreament reference after for end!
      shared_ptr<Handle> handle = handles.find(fd)->second;

      assert(handle->get_eventpointer()->get_fd() == fd);
      assert(handle->get_state() != EXECUTOR_STATE_DEL);
      //handle revents
      handle->handle_event(&(revents.data()[i]));
    }
  }
}

