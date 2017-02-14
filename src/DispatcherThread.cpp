#include <boost/bind.hpp>

#include <MutexLocker.h>
#include <DispatcherThread.h>

DispatcherThread::DispatcherThread() :
    DispatcherThread("DispatcherThread") {
}

DispatcherThread::DispatcherThread(const string& name) :
    mutex_(),
    cond_(mutex_),
    thread_(boost::bind(&DispatcherThread::threadFunc, this), name),
    dispatch_(NULL),
    started_(false) {
}

bool DispatcherThread::join() {
  return thread_.join();
}

bool DispatcherThread::MainThread() {
  return thread_.MainThread();
}

Dispatcher *DispatcherThread::start() {
  assert(!started_);
  thread_.start();

  {
    MutexLocker lock(mutex_);
    while (dispatch_ == NULL){
      cond_.wait();
    }
  }

  started_ = true;
  return dispatch_;
}

bool DispatcherThread::stop() {
  return thread_.stop();
}

void DispatcherThread::setName(const string name) {
  return thread_.setName(name);
}

string DispatcherThread::getName() {
  return thread_.getName();
}

DispatcherThread::~DispatcherThread() {

}

void DispatcherThread::threadFunc() {
  Dispatcher dispatch;

  {
    MutexLocker lock(mutex_);
    dispatch_ = &dispatch;
    cond_.notify();
  }
  //or other callback if have.
  dispatch_->dispatch();
}
