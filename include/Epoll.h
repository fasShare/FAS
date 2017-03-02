#ifndef FAS_EPOLL_H
#define FAS_EPOLL_H
#include <sys/epoll.h>
#include <vector>


#include <Types.h>


#include <boost/noncopyable.hpp>

class Poller;
class Events;

/*!
 * \brief The Epoll class
 * Encapsulation of epoll_ctl, epoll_create, epoll_wait
 */
class Epoll : boost::noncopyable {
public:
  Epoll();
  ~Epoll();

  bool pollerEventsAdd(Events* events);
  bool pollerEventsMod(Events* events);
  bool pollerEventsDel(Events* events);
  Timestamp pollerLoop(std::vector<Events> &events, int timeout);

private:
  bool eventCtl(int op, int fd, EpollEvent* event);
  bool eventAdd(Socket_t fd, EpollEvent* event);
  bool eventDel(Socket_t fd, EpollEvent* event);
  bool eventMod(Socket_t fd, EpollEvent* event);
  int loopWait(EpollEvent* events, int maxevents, int timeout);

  int epoll_fd_;
  std::vector<EpollEvent> revents_;
  int maxNum_;
  const int addStep_;
};

#endif // FAS_EPOLL_H
