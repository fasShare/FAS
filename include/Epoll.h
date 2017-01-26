#ifndef FAS_EPOLL_H
#define FAS_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "Poller.h"
#include "Socket.h"
#include "Types.h"
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
  bool eventCtl(int op, int fd, Epoll_Event* event);
  bool eventAdd(Socket_t fd, Epoll_Event* event);
  bool eventDel(Socket_t fd, Epoll_Event* event);
  bool eventMod(Socket_t fd, Epoll_Event* event);
  int loopWait(Epoll_Event* events, int maxevents, int timeout);

  bool pollerEventsAdd(Events* events);
  bool pollerEventsMod(Events* events);
  bool pollerEventsDel(Events* events);
  int pollerLoop(vector<Events> &events, int max_events, int timeout);

private:
  int epoll_fd_;
  vector<Epoll_Event> revents_;
};

#endif // FAS_EPOLL_H
