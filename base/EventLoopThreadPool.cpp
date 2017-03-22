#include <memory>


#include <EventLoopThreadPool.h>
#include <EventLoop.h>


#include <boost/bind.hpp>

using fas::EventLoopThreadPool;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop,
                                         int threadNum) :
    EventLoopThreadPool(baseloop,
                        threadNum,
                        "EventLoopThreadPool") {

}

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop,
                                         int threadNum,
                                         std::string name) :
  threadNum_(threadNum),
  name_(name),
  threads_(),
  loops_(),
  baseloop_(baseloop),
  started_(false),
  next_(0),
  tid_(gettid()) {
  assert(baseloop_  && (threadNum_ >= 0));

  threads_.reserve(threadNum_);
  loops_.reserve(threadNum_);
}

void EventLoopThreadPool::updateThreadNum(int newNum) {
  assertInOwnerThread();
  assert(!started_);
  threadNum_ = newNum;
}

int EventLoopThreadPool::getThreadNum() {
  return threadNum_;
}

void EventLoopThreadPool::updateName(const std::string& newName) {
  assertInOwnerThread();
  assert(!started_);
  name_ = newName;
}

std::string EventLoopThreadPool::getName() {
  return name_;
}

void EventLoopThreadPool::assertInOwnerThread() {
  assert(gettid() == tid_);
}

bool EventLoopThreadPool::start() {
  assertInOwnerThread();
  assert(!started_);
  for(int i = 0; i < threadNum_; i++) {
    std::shared_ptr<fas::EventLoopThread> thread = std::make_shared<fas::EventLoopThread>();
    loops_.push_back(thread->start());
    threads_.push_back(thread);
  }
  started_ = true;
  //if error
  return started_;
}

fas::EventLoop *EventLoopThreadPool::getNextEventLoop() {
  assertInOwnerThread();
  fas::EventLoop* loop = baseloop_;
  if (!loops_.empty()) {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (next_ >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

EventLoopThreadPool::~EventLoopThreadPool() {
  if (started_) {
    for (auto iter = threads_.begin(); iter < threads_.end(); iter++) {
      (*iter)->join();
    }
  }
  started_ = false;
}
