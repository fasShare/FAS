#ifndef FAS_THREAD_H
#define FAS_THREAD_H

#include <pthread.h>
#include <boost/function.hpp>
#include <string>
#include <syscall.h>

using std::string;
using boost::function;

class Thread {
public:
  Thread(function<void ()> threadFunc);
  Thread(function<void ()> threadFunc, string name);
  bool join();
  bool isMainThread();
  bool startRun();
  string getThreadName();
  void setThreadName(const string& name);

private:
  friend void* runThreadFunc(void *);
  void ThreadFunc();

  pid_t tid_;
  pthread_t threadId_;
  string name_;
  function<void ()> threadFunc_;
};

void* runThreadFunc(void *);
#define gettid() (::syscall(SYS_gettid))

#endif //FAS_THREAD_H
