#ifndef FAS_THREAD_H
#define FAS_THREAD_H
#include <pthread.h>
#include <string>
#include <syscall.h>


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>


class Thread : boost::noncopyable {
public:
  Thread();
  Thread(const std::string& name);
  Thread(boost::function<void ()> threadFunc);
  Thread(boost::function<void ()> threadFunc, const std::string name);

  bool setThreadFunc(boost::function<void ()> threadFunc);

  bool join();
  bool MainThread();

  bool start();

  std::string getName();
  void setName(const std::string& name);

  ~Thread();

private:
  friend void* run(void *);
  void ThreadFunc();

  pid_t tid_;
  pthread_t threadId_;
  std::string name_;
  boost::function<void ()> threadFunc_;
};

void* run(void *);
#define gettid() (::syscall(SYS_gettid))

#endif //FAS_THREAD_H
