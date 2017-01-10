#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "Poller.h"
#include "Socket.h"
#include "NetBaseTypes.h"
#include "Events.h"

class Poller;
class Events;
using namespace std;

/**
*EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLET
*/

class Epoll : public Poller
{
private:
  int epfd;
  vector<Epoll_Event> revents;
public:
  Epoll();
  ~Epoll();
  bool event_ctl(int op, int fd, Epoll_Event* event);
  bool event_add(int fd, Epoll_Event* event);
  bool event_del(int fd, Epoll_Event* event);
  bool event_mod(int fd, Epoll_Event* event);
  bool event_add(Socket fd, Epoll_Event* event);
  bool event_del(Socket fd, Epoll_Event* event);
  bool event_mod(Socket fd, Epoll_Event* event);
  int loop_wait(Epoll_Event* events, int maxevents, int timeout);

  virtual bool Poller_event_add(Events* events);
  virtual bool Poller_event_mod(Events* events);
  virtual bool Poller_event_del(Events* events);
  virtual int Poller_loop(vector<Events> &events, int max_events, int timeout);
};

#endif // EPOLL_H
