#ifndef FAS_EXECUTOR_H
#define FAS_EXECUTOR_H
#include <iostream>

#include <EventLoop.h>

#include <boost/function.hpp>

namespace fas {

class Events;
class Timestamp;

class Handle {
public:
    typedef boost::function<bool (const Events& event)> EventCheckFunc;
    typedef boost::function<void (const fas::Events&, Timestamp)> EventHandleFunc;
    enum state {
        STATE_ADD = 1,
        STATE_MOD,
        STATE_DEL,
        STATE_LOOP,
        STATE_NEW,
    };

    Handle(EventLoop* loop, Events event);
    virtual ~Handle();

    bool reset();
    bool reinit(EventLoop* loop, const Events& events);

    EventLoop* getLoop() const;

    int fd() const;
    Events* getEvent() const;
    void updateEvent(Events& event);

    void enableWrite();
    void disableWrite();
    void enableRead();
    void disableRead();

    void setState(uint8_t state);
    uint8_t getState();

    void setHandleRead(const EventHandleFunc& handle_read);
    void setHandleWrite(const EventHandleFunc& handle_write);
    void setHandleError(const EventHandleFunc& handle_error);
    void setHandleClose(const EventHandleFunc& handle_close);

    void handleEvent(const Events&, Timestamp);
private:
    EventLoop* loop_;
    Events *events_;
    unsigned char state_;

    boost::function<void (Events, Timestamp)> handle_read_event_;
    boost::function<void (Events, Timestamp)> handle_write_event_;
    boost::function<void (Events, Timestamp)> handle_error_event_;
    boost::function<void (Events, Timestamp)> handle_close_event_;
};

}
#endif // FAS_EXECUTOR_H
