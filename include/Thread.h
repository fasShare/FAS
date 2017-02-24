#ifndef FAS_THREAD_H
#define FAS_THREAD_H
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <boost/function.hpp>
#include <string>
#include <syscall.h>
#include <EventLoop.h>

using std::string;

class Thread : boost::noncopyable {
public:
  Thread();
  Thread(const string& name);
  Thread(boost::function<void ()> threadFunc);
  Thread(boost::function<void ()> threadFunc, string name);

  bool setThreadFunc(boost::function<void ()> threadFunc);

  bool join();
  bool MainThread();

  bool start();
  bool stop();

  string getName();
  void setName(const string& name);

  ~Thread();

private:
  friend void* run(void *);
  void ThreadFunc();

  pid_t tid_;
  pthread_t threadId_;
  string name_;
  boost::function<void ()> threadFunc_;
};

void* run(void *);
#define gettid() (::syscall(SYS_gettid))

#endif //FAS_THREAD_H
