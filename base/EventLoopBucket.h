#ifndef FAS_EVENTLOOPBUCKET_H
#define FAS_EVENTLOOPBUCKET_H
#include <map>
#include <vector>
#include <unistd.h>
#include <atomic>

#include <EventLoop.h>
#include <Mutex.h>

namespace fas {

class EventLoop;

class EventLoopBucket {
public:
    bool addEventLoop(long tid, EventLoop* loop);
    EventLoop *getNextLoop();
    static EventLoopBucket *Instance();
private:
    EventLoopBucket();
    Mutex mutex_;
    std::map<long, EventLoop *> loops_;
    std::vector<EventLoop *> nextLoops_;
    std::atomic<size_t> next_;
    static EventLoopBucket *instance_;
};

}

#endif //FAS_EVENTLOOPBUCKET_H
