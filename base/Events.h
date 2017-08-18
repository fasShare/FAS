#ifndef FAS_EVENTS_H
#define FAS_EVENTS_H
#include <iostream>
#include <sys/epoll.h>
#include <sys/poll.h>

#include <boost/static_assert.hpp>

namespace fas {

BOOST_STATIC_ASSERT_MSG(EPOLLIN == POLLIN, "EPOLLIN != POLLIN");
BOOST_STATIC_ASSERT_MSG(EPOLLOUT == POLLOUT, "EPOLLOUT != POLLOUT");
BOOST_STATIC_ASSERT_MSG(EPOLLPRI == POLLPRI, "EPOLLPRI != POLLPRI");
BOOST_STATIC_ASSERT_MSG(EPOLLERR == POLLERR, "EPOLLERR != POLLERR");
BOOST_STATIC_ASSERT_MSG(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP != POLLRDHUP");

const uint32_t kNoneEvent = 0;
const uint32_t kReadEvent = POLLIN | POLLPRI;
const uint32_t kWriteEvent = POLLOUT;

class Events {
public:
    Events(const int fd = -1, uint32_t events = kNoneEvent);
    Events(const Events& events);
    Events(const struct epoll_event& events);
    Events(const struct pollfd& events);

    bool reset();

    int getFd() const;
    uint32_t getEvents() const;
    void updateEvents(uint32_t events);
    void addEvent(uint32_t event);
    void deleteEvent(uint32_t event);

    bool contains(uint32_t events) const;
    bool intersect(uint32_t events) const;
    struct epoll_event epollEvents();

    struct pollfd pollEvents();

    friend std::ostream& operator<<(std::ostream& os, Events& events);
private:
    int fd_;
    uint32_t events_;
};

std::ostream& operator<<(std::ostream& os, Events& events);

}
#endif // FAS_EVENTS_H
