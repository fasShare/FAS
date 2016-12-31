#ifndef EVENTS_H
#define EVENTS_H
#include <iostream>
#include "NetBaseTypes.h"

using namespace std;

class Events
{
private:
 int fd;
 int events;
public:
  Events(int fd, int events);
  Events(const Epoll_Event& events);
  Events() = default;
   ~Events();
  int Events_get_fd();
  int Events_set_fd(int fd);
  int Events_get_events();
  int Events_set_events(int events);
  Epoll_Event Events_get_epollevents();
  friend ostream& operator<<(ostream& os, Events& events);
};

#endif // EVENTS_H
