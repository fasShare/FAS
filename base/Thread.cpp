#include <exception>


#include <Thread.h>
#include <Log.h>

using fas::Thread;

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
    LOGGER_SYSERR << "ERROR pthread_join : " <<  ::strerror(errno) << fas::Log::CLRF;
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
    LOGGER_SYSERR << "ERROR pthread_create : " << ::strerror(errno) << fas::Log::CLRF;
    return false;
  }
  return true;
}

void* fas::run(void *obj) {
  Thread *thread = static_cast<Thread*>(obj);
  thread->ThreadFunc();
  return NULL;
}

void Thread::ThreadFunc() {
  if (!this->threadFunc_) {
    LOGGER_ERROR << "Thread no Function object!" << fas::Log::CLRF;
    return;
  }
  try {
    threadFunc_();
  } catch (const std::exception &ex) {
    LOGGER_ERROR << ex.what() << fas::Log::CLRF;
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


