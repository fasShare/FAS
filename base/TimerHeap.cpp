#include <algorithm>


#include <TimerHeap.h>
#include <Timer.h>
#include <Log.h>

fas::TimerHeap::TimerHeap() :
    timers_(),
    earlistExpiration_(nullptr),
    earlistChange_(false) {
}

uint fas::TimerHeap::getTimerNum() const {
    return timers_.size();
}

bool fas::TimerHeap::getEarlistChange() const {
    return earlistChange_;
}

fas::Timestamp fas::TimerHeap::getEarlistExpiration() const {
    return earlistExpiration_->getExpiration();
}

void fas::TimerHeap::getExpiredTimers(std::vector<std::pair<fas::Timestamp,
        fas::Timer*>>& expired,
        fas::Timestamp now) {
    auto sentry(std::make_pair(now, reinterpret_cast<fas::Timer*>(UINTPTR_MAX)));
    auto end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);
}

void fas::TimerHeap::restartIntervalTimer(std::vector<std::pair<fas::Timestamp, fas::Timer*>>& expired) {
    bool earlistChangeTemp = false;
    for (auto iter = expired.begin(); iter != expired.end(); iter++) {
        if (iter->second->getRepeat() &&
                (iter->second->getState() != fas::Timer::STATE::DELETED)) {
            iter->second->restart(fas::Timestamp::now());
            addTimer(iter->second);
            if (!earlistChangeTemp) {
                if (earlistChange_) {
                    earlistChangeTemp = earlistChange_;
                }
            }
        } else {
            iter->second->setState(fas::Timer::STATE::DELETED);
            delete iter->second;
            iter->second = nullptr;
        }
    } // for
    earlistChange_ = earlistChangeTemp;
}

bool fas::TimerHeap::addTimer(fas::Timer *timer) {
    assert(timer->getExpiration().isvalid());
    TimerHeapSetRetType ret = timers_.insert(std::make_pair(timer->getExpiration(), timer));
    if (!ret.second) {
        return false;
    }
    if ((earlistExpiration_ == nullptr)) {
        earlistExpiration_ = timer;
        earlistChange_ = true;
    } else {
        earlistChange_ = false;
        if (earlistExpiration_ != timers_.begin()->second) {
            earlistExpiration_ = timers_.begin()->second;
            earlistChange_ = true;
        }
    }
    return true;
}

void fas::TimerHeap::delTimer(fas::Timer *timer) {
    TimerHeapIter iter = timers_.find(std::make_pair(timer->getExpiration(), timer));
    if (iter == timers_.end()) {
        LOGGER_ERROR("This timer which was deleted not exist.");
        return;
    }
    size_t ret = timers_.erase(std::make_pair(timer->getExpiration(), timer));
    if (ret != 1) {
        LOGGER_ERROR("ret != 1");
        return;
    }
    if (timers_.begin() != timers_.end()) {
        earlistChange_ = false;
        if (earlistExpiration_ != timers_.begin()->second) {
            earlistExpiration_ = timers_.begin()->second;
            earlistChange_ = true;
        }
    }

    delete timer;
    timer = nullptr;
}

fas::TimerHeap::~TimerHeap() {
    std::vector<std::pair<fas::Timestamp, fas::Timer *>> destroy;
    std::copy(timers_.begin(), timers_.end(), std::back_inserter(destroy));
    timers_.clear();
    for (auto iter = destroy.begin(); iter != destroy.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
}
