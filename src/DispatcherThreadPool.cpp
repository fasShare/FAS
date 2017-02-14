#include <boost/bind.hpp>

#include <DispatcherThreadPool.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

DispatcherThreadPool::DispatcherThreadPool(Dispatcher *baseDispatch, int threadNum) :
    DispatcherThreadPool(baseDispatch, threadNum, "DispatcherThreadPool"){

}

DispatcherThreadPool::DispatcherThreadPool(Dispatcher *baseDispatch, int threadNum, string name) :
    threadNum_(threadNum),
    name_(name),
    baseDispatch_(baseDispatch),
    started_(false),
    next_(0) {
    assert(baseDispatch_  && (threadNum_ >= 0));

  threads_.reserve(threadNum_);
  dispatchers_.reserve(threadNum_);
}

void DispatcherThreadPool::setThreadNum(int threadNum) {
  threadNum_ = threadNum;
}

int DispatcherThreadPool::getThreadNum() {
  return threadNum_;
}

void DispatcherThreadPool::setName(const string& name) {
  name_ = name;
}

string DispatcherThreadPool::getName() {
  return name_;
}

bool DispatcherThreadPool::start() {
  assert(!started_);

  for(int i = 0; i < threadNum_; i++) {
    DispatcherThread *thread = new DispatcherThread;
    dispatchers_.push_back(thread->start());
    threads_.push_back(thread);
  }
  started_ = true;
  //if error
  return true;
}

Dispatcher *DispatcherThreadPool::getNextDispatcher() {
  baseDispatch_->assertInDispatch();
  Dispatcher* dispatch = baseDispatch_;

  if (!dispatchers_.empty()) {
    // round-robin
    dispatch = dispatchers_[next_];
    ++next_;
    if (next_ >= dispatchers_.size()) {
      next_ = 0;
    }
  }
  return dispatch;
}

DispatcherThreadPool::~DispatcherThreadPool() {
  for (auto iter = threads_.begin(); iter < threads_.end(); iter++) {
    (*iter)->stop();
  }
  for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
    delete *iter;
  }
  for(auto iter = dispatchers_.begin(); iter < dispatchers_.end(); iter++) {
    delete *iter;
  }
}
