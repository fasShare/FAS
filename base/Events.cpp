#include <strings.h>

#include <Events.h>

fas::Events::Events(const int fd, uint32_t events) :
    fd_(fd),
    events_(events) {
}

fas::Events::Events(const Events& events) :
    Events(events.fd_, events.events_) {
}

//下面这两个构造主要用与polling返回时
fas::Events::Events(const struct epoll_event& events) :
    Events(events.data.fd, events.events){
}

fas::Events::Events(const struct pollfd& events) :
    Events(events.fd, events.revents) {
}

bool fas::Events::reset() {
    fd_ = -1;
    events_ = kNoneEvent;
    return true;
}

int fas::Events::getFd() const{
    return fd_;
}

uint32_t fas::Events::getEvents() const{
    return events_;
}

void fas::Events::updateEvents(uint32_t events) {
    events_ = events;
}

void fas::Events::addEvent(uint32_t event) {
    events_ |= event;
}

void fas::Events::deleteEvent(uint32_t event) {
    events_ &= ~event;
}

bool fas::Events::contains(uint32_t events) const {
    return ((events_ & events) == events);
}

bool fas::Events::intersect(uint32_t events) const {
    return ((events_ & events) != 0);
}

struct epoll_event fas::Events::epollEvents() {
    struct epoll_event events;
    bzero(&events, sizeof(struct epoll_event));
    events.data.fd = this->getFd();
    events.events = this->getEvents();
    return events;
}

struct pollfd fas::Events::pollEvents() {
    struct pollfd events;
    bzero(&events, sizeof(struct pollfd));
    events.fd = this->getFd();
    events.events = this->getEvents();
    return events;
}

std::ostream& fas::operator<<(std::ostream& os, Events& events) {
    std::string state = "";
    if ((events.events_ & kReadEvent) == kReadEvent) {
        state = "kReadEvent";
    }
    if ((events.events_ & kWriteEvent) == kWriteEvent) {
        if (!state.empty()) {
            state += "|";
        }
        state = "kWriteEvent";
    }
    if (state.empty()) {
        state = "kNoneEvent";
    }
    os << "fd:" << events.getFd() << ", event:" << state;
    return os;
}
