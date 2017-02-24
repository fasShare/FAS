#include <Thread.h>
#include <exception>
#include <Log.h>

using std::exception;

Thread::Thread(){
}

Thread::Thread(const string& name) :
    name_(name){

}

Thread::Thread(boost::function<void ()> threadFunc) :
    Thread(threadFunc, "") {
}

Thread::Thread(boost::function<void ()> threadFunc,
               string name) :
  threadId_(0),
  name_(name),
  threadFunc_(threadFunc) {
}

bool Thread::setThreadFunc(boost::function<void ()> threadFunc) {
  if (threadFunc_)
    return false;
  threadFunc_ = threadFunc;
  return true;
}

bool Thread::join() {
  errno = ::pthread_join(threadId_, NULL);
  if (errno != 0) {
    LOG_SYSERR(string("ERROR pthread_join : ") + strerror(errno));
    return false;
  }
  return true;
}

bool Thread::MainThread() {
  return gettid() == ::getpid();
}

bool Thread::start() {
  errno = ::pthread_create(&threadId_, NULL, &run, this);
  if (errno != 0) {
    LOG_SYSERR(string("ERROR pthread_create : ") + strerror(errno));
    return false;
  }
  return true;
}

bool Thread::stop() {
  errno = ::pthread_cancel(threadId_);
  if (errno != 0) {
    LOG_SYSERR(string("ERROR pthread_cancel : ") + strerror(errno));
    return false;
  }
  return true;
}

void* run(void *obj) {
  Thread *thread = static_cast<Thread*>(obj);
  thread->ThreadFunc();
  return NULL;
}

void Thread::ThreadFunc() {
  if (!this->threadFunc_) {
    LOG_ERROR("Thread no Function object!");
    return;
  }
  try {
    threadFunc_();
  } catch (const exception &ex) {
    LOG_ERROR(ex.what());
  }
}

string Thread::getName() {
  return name_;
}

void Thread::setName(const string& name) {
  this->name_ = name;
}

Thread::~Thread() {

}


