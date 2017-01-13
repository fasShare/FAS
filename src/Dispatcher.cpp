#include <Dispatcher.h>
#include <Timestamp.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


Dispatcher::Dispatcher():
                        poll_(NULL){
  revents_.reserve(kInitMaxHandle_);
  poll_.reset(new Poller);
  assert(poll_);
}


bool Dispatcher::updateHandle(shared_ptr<Handle> handle) {
  MutexLocker lock(mutex_);(void)lock;
  update_handles_.insert({handle->get_event().get_fd(), handle});
  return true;
}


bool Dispatcher::addHandle(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_ADD);
  return updateHandle(handle);
}
bool Dispatcher::addHandle(Handle* handle) {
  shared_ptr<Handle> handle_ptr(handle);
  return addHandle(handle_ptr);
}

//FIXME:mod by fd
bool Dispatcher::modHandle(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_MOD);
  return updateHandle(handle);
}
bool Dispatcher::modHandle(Handle* handle) {
  //shared_ptr<Handle> handle_ptr = handles.find(handle->get_event().get_fd())->second;
  shared_ptr<Handle> handle_ptr = get_handle_shared_ptr(handle);
  return modHandle(handle_ptr);
}

//FIXME:del by fd
bool Dispatcher::delHandle(shared_ptr<Handle> handle) {
  handle->set_state(EXECUTOR_STATE_DEL);
  return updateHandle(handle);
}
bool Dispatcher::delHandle(Handle* handle) {
  //shared_ptr<Handle> handle_ptr = handles.find(handle->get_event().get_fd())->second;
  shared_ptr<Handle> handle_ptr = get_handle_shared_ptr(handle);
  return delHandle(handle_ptr);
}


shared_ptr<Handle>  Dispatcher::get_handle_shared_ptr(int fd) {
  map<int, shared_ptr<Handle>>::iterator index =  handles_.find(fd);
  return index->second;
}

shared_ptr<Handle>  Dispatcher::get_handle_shared_ptr(Handle *handle) {
  map<int, shared_ptr<Handle>>::iterator index =  handles_.find(handle->get_event().get_fd());
  return index->second;
}


void Dispatcher::loop() {

  //only defined once
  Timestamp loop_ret_time;
  while (true) {

    for(map<int, shared_ptr<Handle>>::iterator index = update_handles_.begin(); \
                index != update_handles_.end(); index++) {

      assert(index->first == \
             index->second->get_eventpointer()->get_fd());

      shared_ptr<Handle> op_handle = index->second;

      if (op_handle->get_state() == EXECUTOR_STATE_ADD) {

        poll_->events_add_(index->second->get_eventpointer());
        //insert
        handles_[index->first] = index->second;
      } else if (op_handle->get_state() == EXECUTOR_STATE_MOD) {

        poll_->events_mod_(index->second->get_eventpointer());
        //mod
        handles_[index->first] = index->second;
      } else if (op_handle->get_state() == EXECUTOR_STATE_DEL) {
        poll_->events_del_(op_handle->get_eventpointer());
        //del
        handles_.erase(op_handle->get_event().get_fd());
      } else {
        //error
        assert(false);
      }
    }
    update_handles_.clear();
    revents_.clear();

    int ret = poll_->loop_(revents_, 20, -1);
    loop_ret_time = Timestamp::now();

    for(int i = 0; i < ret; i++) { 
      int fd = revents_.data()[i].get_fd();
     
      //handle will decreament reference after for end!
      shared_ptr<Handle> handle = handles_.find(fd)->second;

      assert(handle->get_eventpointer()->get_fd() == fd);
      assert(handle->get_state() != EXECUTOR_STATE_DEL);
      //handle revents
      handle->handleEvent(&(revents_.data()[i]), loop_ret_time);
    }
  }
}

