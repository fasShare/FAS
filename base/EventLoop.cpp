#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <new>

#include <Log.h>
#include <Poller.h>
#include <Poll.h>
#include <Epoll.h>
#include <Socket.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Thread.h>
#include <Handle.h>
#include <Events.h>
#include <MutexLocker.h>
#include <TimersScheduler.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

std::atomic<int> fas::EventLoop::count_(0);

fas::EventLoop::EventLoop() :
    EventLoop(new (std::nothrow) EpollFactory) {
}

fas::EventLoop::EventLoop(PollerFactory *pollerFactory) :
    poll_(nullptr),
    pollDelayTime_(200),
    pollerFactory_(pollerFactory),
    revents_(),
    handles_(),
    updates_(),
    mutex_(),
    cond_(mutex_),
    tid_(gettid()),
    wakeUpFd_(createEventfd()),
    wakeUpHandle_(new Handle(this, Events(wakeUpFd_, kReadEvent))),
    functors_(),
    runningFunctors_(false),
    timerScheduler_(nullptr),
    quit_(false) {
    LOGGER_TRACE("Before EventLoop.");
    poll_ = pollerFactory_->getPoller();
    poll_ = new (std::nothrow) Epoll();
    if (!poll_) {
        LOGGER_SYSERR("New Poller error!");
    }
    count_++;
    wakeUpHandle_->setHandleRead(boost::bind(&EventLoop::handWakeUp, this, _1, _2));
    addSHandle(wakeUpHandle_);
    timerScheduler_ = new (std::nothrow) TimersScheduler(this);
    if (timerScheduler_ == nullptr) {
        LOGGER_SYSERR("New TimersScheduler error.");
    }
	LOGGER_TRACE("wakeup fd = " << wakeUpFd_);
}

long fas::EventLoop::getTid() const{
    return tid_;
}

int fas::EventLoop::getEventLoopNum() const {
    return count_;
}

bool fas::EventLoop::updateHandle(SHandlePtr handle) {
    assert(handle->getLoop() == this);
    //It'll insert() fail when the key is same.
    updates_[handle->fd()] = handle;
    if (isInLoopOwnerThread()) {
        return updateHandlesUnlock();
    }
    return true;
}

bool fas::EventLoop::addHandle(HandlePtr handle) {
    assert(handle->getState() == Handle::state::STATE_NEW);
    MutexLocker lock(mutex_);(void)lock;
    handle->setState(Handle::state::STATE_ADD);
    return updateHandle(boost::shared_ptr<Handle>(handle));
}

bool fas::EventLoop::addSHandle(SHandlePtr handle) {
    assert(handle->getState() == Handle::state::STATE_NEW);
    MutexLocker lock(mutex_);(void)lock;
    handle->setState(Handle::state::STATE_ADD);
    return updateHandle(handle);
}

bool fas::EventLoop::modHandle(HandlePtr handle) {
    MutexLocker lock(mutex_);(void)lock;
    if (handles_.find(handle->fd()) == handles_.end()) {
        return false;
    }
    SHandlePtr mod = handles_.find(handle->fd())->second;
    mod->setState(Handle::state::STATE_MOD);
    return updateHandle(mod);
}

bool fas::EventLoop::modSHandle(SHandlePtr handle) {
    MutexLocker lock(mutex_);(void)lock;
    if (handles_.find(handle->fd()) == handles_.end()) {
        LOGGER_ERROR("handles_.find(handle->fd()) == handles_.end()");
        return false;
    }
    SHandlePtr mod = handles_.find(handle->fd())->second;
    mod->setState(Handle::state::STATE_MOD);
    return updateHandle(mod);
}

bool fas::EventLoop::delHandle(HandlePtr handle) {
    MutexLocker lock(mutex_);(void)lock;
    if (handles_.find(handle->fd()) == handles_.end()) {
        return false;
    }
    SHandlePtr del = handles_.find(handle->fd())->second;
    del->setState(Handle::state::STATE_DEL);
    return updateHandle(del);
}

bool fas::EventLoop::delSHandle(SHandlePtr handle) {
    MutexLocker lock(mutex_);(void)lock;
    if (handles_.find(handle->fd()) == handles_.end()) {
        LOGGER_ERROR("handles_.find(handle->fd()) == handles_.end()");
        return false;
    }
    SHandlePtr del = handles_.find(handle->fd())->second;
    del->setState(Handle::state::STATE_DEL);
    return updateHandle(del);
}

void fas::EventLoop::addTimer(Timer *timer) {
    assertInOwnerThread();
    timerScheduler_->addTimer(timer);
}

void fas::EventLoop::delTimer(Timer *timer) {
    assertInOwnerThread();
    timerScheduler_->delTimer(timer);
}

bool fas::EventLoop::updateHandlesUnlock() {
    for(auto cur = updates_.begin(); cur != updates_.end(); cur++) {
        SHandlePtr handle = cur->second;
        Events* event = handle->getEvent();

        if (handle->getState() == Handle::state::STATE_ADD) {
            poll_->EventsAdd(event);
            handle->setState(Handle::state::STATE_LOOP);
            handles_[cur->first] = handle;
        } else if (handle->getState() == Handle::state::STATE_MOD) {
            poll_->EventsMod(event);
            handle->setState(Handle::state::STATE_LOOP);
            handles_[cur->first] = handle;
        }else if (handle->getState() == Handle::state::STATE_DEL) {
            poll_->EventsDel(event);
            size_t n = handles_.erase(handle->fd());
            assert(n == 1);
        } else {
            assert(false);
        }
    }
    updates_.clear();
    return true;
}

bool fas::EventLoop::updateHandles() {
    MutexLocker lock(mutex_);(void)lock;
    return updateHandlesUnlock();
}

bool fas::EventLoop::isInLoopOwnerThread() {
    return (gettid() == tid_);
}

void fas::EventLoop::assertInOwnerThread() {
    assert(isInLoopOwnerThread());
}

void fas::EventLoop::wakeUp() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeUpFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOGGER_ERROR("EventLoop::wakeup() writes " << n << " bytes instead of 8");
    }

}

void fas::EventLoop::handWakeUp(Events event, Timestamp time) {
    boost::ignore_unused(event, time);
    uint64_t one = 1;
    ssize_t n = ::read(wakeUpFd_, &one, sizeof one);
    if (n != sizeof one){
        LOGGER_ERROR("EventLoop::handleRead() reads " << n << " bytes instead of 8");
    }
}

void fas::EventLoop::resetOwnerTid() {
	tid_ = gettid();
}

int fas::createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOGGER_SYSERR("Failed in eventfd!");
        ::abort();
    }
    return evtfd;
}


void fas::EventLoop::runInLoop(const Functor& cb) {
    if (isInLoopOwnerThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void fas::EventLoop::queueInLoop(const Functor& cb) {
    {
        MutexLocker lock(mutex_);
        functors_.push_back(cb);
    }
    // if isInLoopOwnerThread() == true, we wake up the Thread is useless.
    // because the statement of program in this Thread is synchronous.
    if (!isInLoopOwnerThread() || runningFunctors_) {
        wakeUp();
    }
}

void fas::EventLoop::runFunctors() {
    std::vector<Functor> functors;
    runningFunctors_ = true;

    {
        MutexLocker lock(mutex_);
        functors.swap(functors_);
    }

    for (size_t i = 0; i < functors.size(); ++i) {
        functors[i]();
        //After the exec of functors, maybe some handle should be destroy    ed before run Functions. 
        if (!updates_.empty()) {
            updateHandles();
        }
    }
    runningFunctors_ = false;
}

void fas::EventLoop::quit() {
    MutexLocker lock(mutex_);
    quit_ = true;
    if(!isInLoopOwnerThread()) {
        wakeUp();
    }
    boost::ignore_unused(lock);
}

bool fas::EventLoop::loop() {
    LOGGER_TRACE("A EventLoop started.");
    assertInOwnerThread();
    Timestamp looptime;
    while (!quit_) {
        if (!updates_.empty()) {
            updateHandles();
        }

        revents_.clear();

        looptime = poll_->Loop(revents_, pollDelayTime_);
        for(std::vector<Events>::iterator iter = revents_.begin();
                iter != revents_.end(); iter++) {
            //handle will decreament reference after for end!
            std::map<int, SHandlePtr>::iterator finditer = handles_.find((*iter).getFd());
            if (finditer == handles_.end()) {
                continue;
            }
            SHandlePtr handle = finditer->second;
            if (handle->getState() != Handle::state::STATE_LOOP) {
                LOGGER_DEBUG("handle'state != STATE_LOOP!");
                continue;
            }
            handle->handleEvent(*iter, looptime);
            //After the exec of handleEvent, maybe some handle should be destroyed before run Functions. 
            if (!updates_.empty()) {
                updateHandles();
            }
        }

        assert(!runningFunctors_);
        runFunctors();
    } 
    return true;
}

fas::EventLoop::~EventLoop() { 
    LOGGER_TRACE("EventLoop will be destroyed in process " << getpid());
    if (timerScheduler_) {
        delete timerScheduler_;
    }
}

