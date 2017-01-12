#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "Poller.h"
#include "Socket.h"
#include "NetBaseTypes.h"
#include "Events.h"

#include <boost/noncopyable.hpp>

class Poller;
class Events;
using namespace std;

/**
*EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLET
*/

class Epoll : boost::noncopyable {
public:
  Epoll();
  ~Epoll();
  bool event_ctl(int op, int fd, Epoll_Event* event);
  bool event_add(Socket_t fd, Epoll_Event* event);
  bool event_del(Socket_t fd, Epoll_Event* event);
  bool event_mod(Socket_t fd, Epoll_Event* event);
  int loop_wait(Epoll_Event* events, int maxevents, int timeout);

  bool poller_events_add(Events* events);
  bool poller_events_mod(Events* events);
  bool poller_events_del(Events* events);
  int poller_loop(vector<Events> &events, int max_events, int timeout);

private:
  int epoll_fd_;
  vector<Epoll_Event> revents_;
};

#endif // EPOLL_H
