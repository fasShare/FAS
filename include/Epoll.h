#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "EventsPoller.h"
#include "Socket.h"
#include "NetBaseTypes.h"
#include "Events.h"

class EventsPoller;
class Events;
using namespace std;

/**
*EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLET
*/

class Epoll : public EventsPoller
{
private:
  int epfd;
  vector<Epoll_Event> revents;
public:
  Epoll();
  ~Epoll();
  bool Epoll_ctl(int op, int fd, Epoll_Event* event);
  bool Epoll_event_add(int fd, Epoll_Event* event);
  bool Epoll_event_del(int fd, Epoll_Event* event);
  bool Epoll_event_mod(int fd, Epoll_Event* event);
  bool Epoll_event_add(Socket fd, Epoll_Event* event);
  bool Epoll_event_del(Socket fd, Epoll_Event* event);
  bool Epoll_event_mod(Socket fd, Epoll_Event* event);
  int Epoll_wait(Epoll_Event* events, int maxevents, int timeout);

  virtual bool Poller_event_add(Events* events);
  virtual bool Poller_event_mod(Events* events);
  virtual bool Poller_event_del(Events* events);
  virtual int Poller_loop(vector<Events> &events, int max_events, int timeout);
};

#endif // EPOLL_H
