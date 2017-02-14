#include <ThreadPool.h>
#include <memory>

using std::make_shared;

ThreadPool::ThreadPool() :
    curThreadNum_(0),
    threadNum_(0),
    name_("ThreadPool"),
    canStart_(false){
}

void ThreadPool::setThreadFunc(boost::function<void ()> func) {
  this->threadFunc_ = func;
  canStart_ = true;
}

void ThreadPool::setThreadNum(int num) {
  this->threadNum_ = num;
}

bool ThreadPool::start() {
  assert(canStart_);
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
  return count != threadNum_ ? false : true;
}

bool ThreadPool::stop() {
    int count = 0;
    for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
      if ((*iter)->stop()) {
        count++;
      } else {
        break;
      }
    }
    return count != threadNum_ ? false : true;
}

ThreadPool::~ThreadPool() {
  for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
      (*iter)->join();
  }
}
