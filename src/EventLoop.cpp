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

int EventLoop::getCount() const {
  return count_;
}

bool EventLoop::updateHandle(Handle *handle) {
  MutexLocker lock(mutex_);(void)lock;
  SHandlePtr handle_ptr(handle);
  updates_.insert({handle_ptr->getEvent()->getFd(), handle_ptr});
  return true;
}


bool EventLoop::addHandle(Handle *handle) {
  assert(handle->getLoop() == this);
  handle->setState(STATE_ADD);
  return updateHandle(handle);
}

//FIXME:mod by fd
bool EventLoop::modHandle(Handle *handle) {
  handle->setState(STATE_MOD);
  return updateHandle(handle);
}

//FIXME:del by fd
bool EventLoop::delHandle(Handle *handle) {
  handle->setState(STATE_DEL);
  return updateHandle(handle);
}

bool EventLoop::addNewToHandles() {
  MutexLocker lock(mutex_);(void)lock;
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

void EventLoop::assertInOwner() {
  assert(gettid() == tid_);
}

void EventLoop::loop() {
  //cout << "Thread tid " << gettid() << " " << tid_ << " will loop." << endl;
  assertInOwner();
  //only defined once
  Timestamp looptime;
  uint waitTime = 0;
  while (true) {
    if (!updates_.empty())
      addNewToHandles();

    updates_.clear();
    revents_.clear();

    if (handles_.size() > 0) {
        cout << "tid : " << gettid() << " handles num " << handles_.size() << endl;
      waitTime = -1;
    } else {
      waitTime = 10;
    }
    int ret = poll_->loop_(revents_, 20, waitTime);
    looptime = Timestamp::now();



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

