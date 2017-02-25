#ifndef FAS_THREADPOOL_H
#define FAS_THREADPOOL_H
#include <vector>
#include <memory>


#include <Thread.h>


#include <boost/noncopyable.hpp>

class ThreadPool : boost::noncopyable{
public:
    ThreadPool(int threadNum, boost::function<void ()> func, const std::string name);
    ~ThreadPool();

    void updateThreadFunc(boost::function<void ()> Func);
    void updateThreadNum(int Num);

    void assertInOwnerThread();

    bool start();

private:
    int curThreadNum_;
    int threadNum_;
    std::vector<std::shared_ptr<Thread>> threads_;
    std::string name_;
    boost::function<void ()> threadFunc_;
    pid_t tid_;
    bool started_;
};

#endif // FAS_THREADPOOL_H

