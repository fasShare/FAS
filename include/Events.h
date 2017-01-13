#ifndef EVENTS_H
#define EVENTS_H
#include <iostream>
#include <Types.h>
#include <boost/static_assert.hpp>
using std::ostream;
using std::endl;

BOOST_STATIC_ASSERT_MSG(EPOLLIN == POLLIN, "EPOLLIN != POLLIN");
BOOST_STATIC_ASSERT_MSG(EPOLLOUT == POLLOUT, "EPOLLOUT != POLLOUT");
BOOST_STATIC_ASSERT_MSG(EPOLLPRI == POLLPRI, "EPOLLPRI != POLLPRI");
BOOST_STATIC_ASSERT_MSG(EPOLLERR == POLLERR, "EPOLLERR != POLLERR");
BOOST_STATIC_ASSERT_MSG(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP != POLLRDHUP");

const events_t kNoneEvent = 0;
const events_t kReadEvent = POLLIN | POLLPRI;
const events_t kWriteEvent = POLLOUT;

class Events {
private:
 int fd_;
 int events_;
public:
  Events(int fd, int events);
  Events(const Epoll_Event& events);
  Events() = default;
   ~Events();
  int get_fd();
  void set_fd(int fd);
  int get_events();
  void set_events(int events);

  bool containsEvents(int events);
  bool containsAtLeastOneEvents(int events);

  Epoll_Event getEpollevents();
};

ostream& operator<<(ostream& os, Events& events);

#endif // EVENTS_H
