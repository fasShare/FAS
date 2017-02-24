#ifndef FAS_THREADPOOL_H
#define FAS_THREADPOOL_H
#include <boost/noncopyable.hpp>
#include <vector>
#include <Thread.h>
#include <memory>

using std::vector;
using std::shared_ptr;

class ThreadPool : boost::noncopyable{
public:
    ThreadPool(int threadNum, boost::function<void ()> func, string name);
    ~ThreadPool();

    void updateThreadFunc(boost::function<void ()> Func);
    void updateThreadNum(int Num);

    void assertInOwner();

    bool start();
    bool stop();

private:
    int curThreadNum_;
    int threadNum_;
    vector<shared_ptr<Thread>> threads_;
    string name_;
    boost::function<void ()> threadFunc_;
    pid_t tid_;
    bool started_;
};

#endif // FAS_THREADPOOL_H

