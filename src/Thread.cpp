#include <Thread.h>
#include <exception>
#include <Log.h>

using std::exception;

Thread::Thread(function<void ()> threadFunc) :
    Thread(threadFunc, "") {
}

Thread::Thread(function<void ()> threadFunc, string name) :
    tid_(0),
    threadId_(0),
    name_(name),
    threadFunc_(threadFunc) {
}

bool Thread::join() {
  errno = ::pthread_join(threadId_, NULL);
  if (errno != 0) {
    LOG_SYSERR(string("ERROR pthread_join : ") + strerror(errno));
    return false;
  }
  return true;
}

bool Thread::isMainThread() {
  return tid_ == ::getpid();
}

bool Thread::startRun() {
  errno = ::pthread_create(&threadId_, NULL, &runThreadFunc, this);
  if(errno != 0) {
    LOG_SYSERR(string("ERROR pthread_create : ") + strerror(errno));
    return false;
  }
  return true;
}

void* runThreadFunc(void *obj) {
  Thread *thread = static_cast<Thread*>(obj);
  thread->ThreadFunc();
  return NULL;
}

void Thread::ThreadFunc() {
  if (!this->threadFunc_) {
    return;
  }
  try {
    threadFunc_();
  } catch (const exception &ex) {
    LOG_ERROR(ex.what());
  }
}

string Thread::getThreadName() {
  return name_;
}

void Thread::setThreadName(const string& name) {
  this->name_ = name;
}


