#include <EventLoopThread.h>
#include <EventLoop.h>
#include <MutexLocker.h>
#include <Log.h>


#include <boost/bind.hpp>

fas::EventLoopThread::EventLoopThread(const std::string& name) :
  mutex_(),
  cond_(mutex_),
  thread_(boost::bind(&EventLoopThread::threadFunc, this), name),
  loop_(NULL),
  started_(false) {
}

bool fas::EventLoopThread::MainThread() {
  return thread_.MainThread();
}

fas::EventLoop *fas::EventLoopThread::start() {
  assert(!started_);
  thread_.start();

  {
    fas::MutexLocker lock(mutex_);
    while (loop_ == NULL){
      cond_.wait();
    }
  }

  started_ = true;
  return loop_;
}

bool fas::EventLoopThread::join() {
  if (started_) {
    loop_->quit();
    thread_.join();;
    started_ = false;
  }
  return !started_;
}

void fas::EventLoopThread::setName(const std::string name) {
  return thread_.setName(name);
}

std::string fas::EventLoopThread::getName() {
  return thread_.getName();
}

fas::EventLoopThread::~EventLoopThread() {
}

void fas::EventLoopThread::threadFunc() {
  LOGGER_TRACE("Thread tid :"  << gettid());
  fas::EventLoop loop;
  loop.assertInOwnerThread();

  {
    fas::MutexLocker lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }
  //or other callback if have.
  loop_->loop();
}
