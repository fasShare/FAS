#include <EventLoop.h>
#include <Timestamp.h>
#include <Thread.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <Log.h>

int EventLoop::count_ = 0;

EventLoop::EventLoop() :
  poll_(NULL),
  mutex_(),
  cond_(mutex_),
  tid_(gettid()) {
  revents_.reserve(kInitMaxHandle_);
  poll_.reset(new Poller);
  assert(poll_);
  count_++;
}

bool EventLoop::updateHandle(HandlePtr handle) {
  MutexLocker lock(mutex_);(void)lock;
  assert(handle->getLoop() == this);
  SHandlePtr handle_ptr(handle);
  updates_.insert({handle_ptr->getEvent()->getFd(), handle_ptr});
  return true;
}


bool EventLoop::addHandle(HandlePtr handle) {
  handle->setState(STATE_ADD);
  return updateHandle(handle);
}

//FIXME:mod by fd
bool EventLoop::modHandle(HandlePtr handle) {
  handle->setState(STATE_MOD);
  return updateHandle(handle);
}

//FIXME:del by fd
bool EventLoop::delHandle(HandlePtr handle) {
  handle->setState(STATE_DEL);
  return updateHandle(handle);
}

bool EventLoop::addNewToHandles() {
  for(auto cur = updates_.begin();
       cur != updates_.end();
       cur++) {

    SHandlePtr handle = cur->second;

    if (handle->getState() == STATE_ADD) {
      poll_->events_add_(handle->getEvent());
      //insert
      handles_[cur->first] = handle;
      handle->setState(STATE_LOOP);
    } else if (handle->getState() == STATE_MOD) {
      poll_->events_mod_(handle->getEvent());
      //mod
      handles_[cur->first] = handle;
      handle->setState(STATE_LOOP);
    } else if (handle->getState() == STATE_DEL) {
      poll_->events_del_(handle->getEvent());
      //del
      handles_.erase(handle->getEvent()->getFd());
    } else {
      //error
      assert(false);
    }
  }
  return false;
}

void EventLoop::loop() {

  //only defined once
  Timestamp looptime;
  while (true) {
    if (!updates_.empty())
      addNewToHandles();

    updates_.clear();
    revents_.clear();

    int ret = poll_->loop_(revents_, 20, -1);
    looptime = Timestamp::now();

    cout << "current dispather own handles num is " << EventLoop::count_ << endl;

    for(int i = 0; i < ret; i++) { 
      int fd = revents_.data()[i].getFd();
     
      //handle will decreament reference after for end!
      SHandlePtr handle = handles_.find(fd)->second;
      if (handle->getState() != STATE_LOOP) {
        LOG_DEBUG("After Loop have handle with state STATE_LOOP! it is unnormal!");
        continue;
      }
      assert(handle->getEvent()->getFd() == fd);
      //handle revents
      handle->handleEvent(&(revents_.data()[i]), looptime);
    }
  }
}

EventLoop::~EventLoop() {
}

