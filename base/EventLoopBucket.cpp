#include <EventLoopBucket.h>
#include <MutexLocker.h>
#include <Log.h>

fas::EventLoopBucket* fas::EventLoopBucket::instance_ = nullptr; 
fas::EventLoopBucket::EventLoopBucket() :
    loops_(),
    nextLoops_(),
    next_(0) {
}

bool fas::EventLoopBucket::addEventLoop(long tid, fas::EventLoop* loop) {
    MutexLocker locker(mutex_);
    if ((loop == NULL) || (loops_.find(tid) != loops_.end())) {
        return false;
    }
    loops_[tid] = loop;
    nextLoops_.push_back(loop);
    LOGGER_TRACE("Num of EventLoop in Bucket is " << nextLoops_.size());
    return true;
}

fas::EventLoop *fas::EventLoopBucket::getNextLoop() {
    MutexLocker locker(mutex_);
    if (nextLoops_.size() == 0) {
        return nullptr;
    }
    if (nextLoops_.size() <= next_) {
        next_ = 0;
    }
    return nextLoops_[next_++];
}

fas::EventLoopBucket *fas::EventLoopBucket::Instance() {
    if (nullptr == instance_) {
        instance_ = new (std::nothrow) EventLoopBucket();
    }
    return instance_;
}
