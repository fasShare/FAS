#ifndef FAS_DISPATHERTHREADPOOL_H
#define FAS_DISPATHERTHREADPOOL_H
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <DispatcherThread.h>
#include <Dispatcher.h>
#include <vector>

using std::vector;

class DispatcherThreadPool : boost::noncopyable {
public:
    DispatcherThreadPool(Dispatcher *baseDispatch, int threadNum);
    DispatcherThreadPool(Dispatcher *baseDispatch, int threadNum, string name);
    ~DispatcherThreadPool();

    void setThreadNum(int threadNum);
    int getThreadNum();

    void setName(const string& name);
    string getName();

    bool start();

    Dispatcher *getNextDispatcher();
private:
    int threadNum_;
    string name_;
    vector<DispatcherThread *> threads_;
    vector<Dispatcher *> dispatchers_;
    Dispatcher *baseDispatch_;
    bool started_;
    size_t next_;
};

#endif // FAS_DISPATHERTHREADPOOL_H

