#include <ThreadPool.h>
#include <memory>

using std::make_shared;

ThreadPool::ThreadPool(int threadNum, boost::function<void ()> func, string name) :
  curThreadNum_(0),
  threadNum_(threadNum),
  threads_(),
  name_(name),
  threadFunc_(func),
  tid_(gettid()),
  started_(false) {
  assert(threadNum >= 0);
}

void ThreadPool::updateThreadFunc(boost::function<void ()> func) {
  assertInOwner();
  assert(!started_);
  this->threadFunc_ = func;
}

void ThreadPool::updateThreadNum(int newNum) {
  assertInOwner();
  assert(!started_);
  assert(newNum >= 0);
  threadNum_ = newNum;
}

void ThreadPool::assertInOwner() {
  assert(gettid() == tid_);
}

bool ThreadPool::start() {
  assertInOwner();
  assert(!started_);
  for(int i = 0; i < threadNum_; i++) {
    threads_.push_back(make_shared<Thread>(threadFunc_));
  }
  int count = 0;
  for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
    if ((*iter)->start()) {
      count++;
    } else {
      break;
    }
  }
  started_ = true;
  return count != threadNum_ ? false : true;
}

bool ThreadPool::stop() {
  assertInOwner();
  assert(started_);
  int count = 0;
  for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
    if ((*iter)->stop()) {
      count++;
    } else {
      break;
    }
  }
  started_ = false;
  return count != threadNum_ ? false : true;
}

ThreadPool::~ThreadPool() {
  started_ = false;
  for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
    (*iter)->join();
  }
}
