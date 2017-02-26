#ifndef FAS_EVENTS_H
#define FAS_EVENTS_H
#include <iostream>


#include <Default.h>


#include <boost/static_assert.hpp>

BOOST_STATIC_ASSERT_MSG(EPOLLIN == POLLIN, "EPOLLIN != POLLIN");
BOOST_STATIC_ASSERT_MSG(EPOLLOUT == POLLOUT, "EPOLLOUT != POLLOUT");
BOOST_STATIC_ASSERT_MSG(EPOLLPRI == POLLPRI, "EPOLLPRI != POLLPRI");
BOOST_STATIC_ASSERT_MSG(EPOLLERR == POLLERR, "EPOLLERR != POLLERR");
BOOST_STATIC_ASSERT_MSG(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP != POLLRDHUP");

const events_t kNoneEvent = 0;
const events_t kReadEvent = POLLIN | POLLPRI;
const events_t kWriteEvent = POLLOUT;

class Events {
public:
  Events(int fd, uint32_t events);
  Events(const Events& events);
  Events(const EpollEvent& events);
   ~Events();

  int getFd() const;
  uint32_t getEvents() const;
  void updateEvents(uint32_t events);

  bool containsEvents(uint32_t events) const;
  bool containsAtLeastOneEvents(uint32_t events) const;

  EpollEvent epollEvents();

private:
  int fd_;
  uint32_t events_;
};

std::ostream& operator<<(std::ostream& os, Events& events);

#endif // FAS_EVENTS_H
