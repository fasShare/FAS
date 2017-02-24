#include <boost/bind.hpp>

#include <EventLoopThreadPool.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop,
                                         int threadNum) :
    EventLoopThreadPool(baseloop,
                        threadNum,
                        "EventLoopThreadPool") {

}

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop,
                                         int threadNum,
                                         string name) :
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
  assertInOwner();
  assert(!started_);
  threadNum_ = newNum;
}

int EventLoopThreadPool::getThreadNum() {
  return threadNum_;
}

void EventLoopThreadPool::updateName(const string& newName) {
  assertInOwner();
  assert(!started_);
  name_ = newName;
}

string EventLoopThreadPool::getName() {
  return name_;
}

void EventLoopThreadPool::assertInOwner() {
  assert(gettid() == tid_);
}

bool EventLoopThreadPool::start() {
  assertInOwner();
  assert(!started_);
  for(int i = 0; i < threadNum_; i++) {
    EventLoopThread *thread = new EventLoopThread();
    loops_.push_back(thread->start());
    threads_.push_back(thread);
  }
  started_ = true;
  //if error
  return true;
}

EventLoop *EventLoopThreadPool::getNextEventLoop() {
  assertInOwner();
  EventLoop* loop = baseloop_;
  cout << __func__ << " : " << loops_.size() << " next :" << next_ << endl;
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
  for (auto iter = threads_.begin();
       iter < threads_.end();
       iter++) {
    (*iter)->stop();
    delete *iter;
  }
}
