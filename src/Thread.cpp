#include <exception>


#include <Thread.h>
#include <Log.h>

Thread::Thread(){
}

Thread::Thread(const std::string& name) :
    name_(name){

}

Thread::Thread(boost::function<void ()> threadFunc) :
    Thread(threadFunc, "") {
}

Thread::Thread(boost::function<void ()> threadFunc,
               const std::string name) :
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
    LOG_SYSERR(std::string("ERROR pthread_join : ") + ::strerror(errno));
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
    LOG_SYSERR(std::string("ERROR pthread_create : ") + ::strerror(errno));
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
  } catch (const std::exception &ex) {
    LOG_ERROR(ex.what());
  }
}

std::string Thread::getName() {
  return name_;
}

void Thread::setName(const std::string& name) {
  this->name_ = name;
}

Thread::~Thread() {

}


