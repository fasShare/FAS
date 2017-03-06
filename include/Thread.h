#ifndef FAS_THREAD_H
#define FAS_THREAD_H
#include <pthread.h>
#include <string>
#include <syscall.h>


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

/*!
 * \brief The Thread class
 * Encapsulation of pthread_create, pthread_join
 */
class Thread : boost::noncopyable {
public:
  Thread();
  Thread(const std::string& name);
  Thread(boost::function<void ()> threadFunc);
  Thread(boost::function<void ()> threadFunc, const std::string name);

  bool setThreadFunc(boost::function<void ()> threadFunc);
  /*!
   * \brief join
   * \return bool
   * pthread_join
   */
  bool join();
  /*!
   * \brief MainThread
   * \return bool
   * judge this thread if
   */
  bool MainThread();

  /*!
   * \brief start
   * \return bool
   * pthread_create(thread_func)
   */
  bool start();

  std::string getName();
  void setName(const std::string& name);

  ~Thread();

private:
  friend void* run(void *);
  void ThreadFunc();


  pthread_t threadId_;       /*!< Id of the thread */
  std::string name_;         /*!< Name of the thread */
  boost::function<void ()> threadFunc_;  /*!< Thread function */
};


/*!
 * only used in multiple threads environment.
 */
template <typename T>
class PthreadSpecificData {
public:
  PthreadSpecificData() {
    pthread_key_create(&key_, &PthreadSpecificData::KeyDataDestroy);
  }

  T& Data() {
    T *ptr = static_cast<T *>(pthread_getspecific(key_));
    if (ptr == NULL) {
      ptr = new T();
      pthread_setspecific(key_, ptr);
    }
    assert(ptr != NULL);
    return *ptr;
  }

  static void KeyDataDestroy(void *data) {
    T *ptr =  static_cast<T *>(data);
    delete ptr;
  }

private:
  pthread_key_t key_;
};


void* run(void *);
#define gettid() (::syscall(SYS_gettid))

#endif //FAS_THREAD_H
