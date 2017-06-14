#ifndef FAS_EPOLL_H
#define FAS_EPOLL_H
#include <sys/epoll.h>
#include <vector>


#include <Poller.h>


#include <boost/noncopyable.hpp>

namespace fas {

class Poller;
class Events;

/*!
 * \brief The Epoll class
 * Encapsulation of epoll_ctl, epoll_create, epoll_wait
 */
class Epoll : public Poller {
public:
typedef struct epoll_event  EpollEvent;
typedef struct pollfd  PollEvent;

  Epoll();
  ~Epoll();

  bool EventsAdd(Events* events);
  bool EventsMod(Events* events);
  bool EventsDel(Events* events);
  Timestamp Loop(std::vector<Events> &events, int timeout);

private:
  bool eventCtl(int op, int fd, EpollEvent* event);
  bool eventAdd(int fd, EpollEvent* event);
  bool eventDel(int fd, EpollEvent* event);
  bool eventMod(int fd, EpollEvent* event);
  int loopWait(EpollEvent* events, int maxevents, int timeout);

  int epoll_fd_;
  std::vector<EpollEvent> revents_;
  int maxNum_;
  const int addStep_;
};

}
#endif // FAS_EPOLL_H
