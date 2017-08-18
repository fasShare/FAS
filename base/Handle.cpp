#include <assert.h>


#include <Handle.h>
#include <Events.h>
#include <Timestamp.h>
#include <Log.h>
#include <Thread.h>


#include <boost/bind.hpp>

fas::Handle::Handle(EventLoop* loop, Events events) {
    reinit(loop, events);
}

bool fas::Handle::reset() {
    loop_ = nullptr;
    state_ = STATE_NEW;
    return events_.reset();
}

bool fas::Handle::reinit(EventLoop* loop, const Events& events) {
    if (nullptr == loop) {
        return false;
    }
    events_ = events;
    loop_ = loop;
    state_ = STATE_NEW;
    return true;
}

fas::EventLoop* fas::Handle::getLoop() const{
    return loop_;
}

int fas::Handle::fd() const {
    return events_.getFd();
}

fas::Events* fas::Handle::getEvent() {
    return &events_;
}

void fas::Handle::enableWrite() {
    events_.addEvent(kWriteEvent);
}

void fas::Handle::disableWrite() {
    events_.deleteEvent(kWriteEvent);
}

void fas::Handle::enableRead() {
    events_.addEvent(kReadEvent);
}

void fas::Handle::disableRead() {
    events_.deleteEvent(kReadEvent);
}

void fas::Handle::setHandleRead(const EventHandleFunc& handle_read) {
    handle_read_event_ = handle_read;
}

void fas::Handle::setHandleWrite(const EventHandleFunc& handle_write) {
    handle_write_event_ = handle_write;
}

void fas::Handle::setHandleError(const EventHandleFunc& handle_error) {
    handle_error_event_ = handle_error;
}

void fas::Handle::setHandleClose(const EventHandleFunc& handle_close) {
    handle_close_event_ = handle_close;
}


void fas::Handle::setState(uint8_t state) {
    state_ = state;
}
uint8_t fas::Handle::getState() {
    return state_;
}

void fas::Handle::handleEvent(const Events& events, Timestamp time) {
    if (events.intersect(POLLHUP) && !events.intersect(POLLIN)) {
        if (handle_close_event_) handle_close_event_(events, time);
    }

    if (events.intersect(POLLERR | POLLNVAL)) {
        if (handle_error_event_)
            handle_error_event_(events, time);
    }

    if (events.intersect(POLLIN | POLLPRI | POLLRDHUP)) {
        if (handle_read_event_)
            handle_read_event_(events, time);
    }

    if (events.intersect(POLLOUT)) {
        if (handle_write_event_)
            handle_write_event_(events, time);
    }

    if (events.intersect(POLLNVAL)) {
        LOGGER_DEBUG("events contains POLLNVAL!");
    }
}

fas::Handle::~Handle() {
    state_ = STATE_DEL;
    events_.reset();
    LOGGER_TRACE("tid: " << gettid() << " handle Destroyed!");
}
