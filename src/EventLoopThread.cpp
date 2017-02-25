#include <EventLoopThread.h>
#include <EventLoop.h>
#include <MutexLocker.h>


#include <boost/bind.hpp>

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

bool EventLoopThread::join() {
  if (started_) {
    loop_->quit();
    thread_.join();;
    started_ = false;
  }
  return !started_;
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
  std::cout << "Thread tid :" << gettid() << std::endl;
  EventLoop loop;
  loop.assertInOwnerThread();

  {
    MutexLocker lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }
  //or other callback if have.
  loop_->loop();
}
