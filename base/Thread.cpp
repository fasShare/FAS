#include <exception>


#include <Thread.h>
#include <Log.h>

fas::Thread::Thread(){
}

fas::Thread::Thread(const std::string& name) :
    name_(name){

}

fas::Thread::Thread(boost::function<void ()> threadFunc) :
    Thread(threadFunc, "") {
}

fas::Thread::Thread(boost::function<void ()> threadFunc,
               const std::string name) :
  threadId_(0),
  name_(name),
  threadFunc_(threadFunc) {
}

bool fas::Thread::setThreadFunc(boost::function<void ()> threadFunc) {
  if (threadFunc_)
    return false;
  threadFunc_ = threadFunc;
  return true;
}

bool fas::Thread::join() {
  errno = ::pthread_join(threadId_, NULL);
  if (errno != 0) {
    LOGGER_SYSERR("ERROR pthread_join : " <<  ::strerror(errno));
    return false;
  }
  return true;
}

bool fas::Thread::MainThread() {
  return gettid() == ::getpid();
}

bool fas::Thread::start() {
  errno = ::pthread_create(&threadId_, NULL, &run, this);
  if (errno != 0) {
    LOGGER_SYSERR("ERROR pthread_create : " << ::strerror(errno));
    return false;
  }
  return true;
}

void* fas::run(void *obj) {
  Thread *thread = static_cast<Thread*>(obj);
  thread->ThreadFunc();
  return NULL;
}

void fas::Thread::ThreadFunc() {
  if (!this->threadFunc_) {
    LOGGER_ERROR("Thread no Function object!");
    return;
  }
  try {
    threadFunc_();
  } catch (const std::exception &ex) {
    LOGGER_ERROR("exception : " << ex.what());
  }
}

std::string fas::Thread::getName() {
  return name_;
}

void fas::Thread::setName(const std::string& name) {
  this->name_ = name;
}

fas::Thread::~Thread() {

}


