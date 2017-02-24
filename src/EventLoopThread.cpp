#include <boost/bind.hpp>

#include <MutexLocker.h>
#include <EventLoopThread.h>

EventLoopThread::EventLoopThread() :
    EventLoopThread("EventLoopThread") {
}

EventLoopThread::EventLoopThread(const string& name) :
  mutex_(),
  cond_(mutex_),
  thread_(boost::bind(&EventLoopThread::threadFunc, this), name),
  loop_(NULL),
  started_(false) {
}

bool EventLoopThread::join() {
  return thread_.join();
}

bool EventLoopThread::MainThread() {
  return thread_.MainThread();
}

EventLoop *EventLoopThread::start() {
  assert(!started_);
  thread_.start();

  {
    MutexLocker lock(mutex_);
    while (loop_ == NULL){
      cond_.wait();
    }
  }

  started_ = true;
  return loop_;
}

bool EventLoopThread::stop() {
  return thread_.stop();
}

void EventLoopThread::setName(const string name) {
  return thread_.setName(name);
}

string EventLoopThread::getName() {
  return thread_.getName();
}

EventLoopThread::~EventLoopThread() {

}

void EventLoopThread::threadFunc() {
  EventLoop loop;

  {
    MutexLocker lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }
  //or other callback if have.
  loop_->loop();
}
