#ifndef FAS_THREADPOOL_H
#define FAS_THREADPOOL_H
#include <vector>
#include <memory>


#include <Thread.h>


#include <boost/noncopyable.hpp>

namespace fas {

/*!
 * \brief The ThreadPool class
 * ThreadPool
 */
class ThreadPool : boost::noncopyable{
public:
    ThreadPool(int threadNum, boost::function<void ()> func, const std::string name);
    ~ThreadPool();
    /*!
     * \brief updateThreadFunc
     * \param Func
     * Change thread function
     */
    void updateThreadFunc(boost::function<void ()> Func);
    /*!
     * \brief updateThreadNum
     * \param Num
     * Change the thread num of this threadPool. but it dosn't work.
     */
    void updateThreadNum(int Num);

    void assertInOwnerThread();
    /*!
     * \brief start
     * \return bool
     * Start this ThreadPool
     */
    bool start();

private:
    int curThreadNum_;      /*!< Current Thread num of this Threadpool */
    int threadNum_;         /*!< Thread num of this Threadpool */
    std::vector<std::shared_ptr<Thread>> threads_;
    std::string name_;
    boost::function<void ()> threadFunc_;  /*!< Thread function */
    pid_t tid_;             /*!< The pid of the thread which own the ThreadPool */
    bool started_;          /*!< If threadPool started started_ is true. otherwise is false */
};

}
#endif // FAS_THREADPOOL_H

