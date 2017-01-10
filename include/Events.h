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
  int get_fd();
  void set_fd(int fd);
  int get_events();
  void set_events(int events);
  Epoll_Event get_epollevents();
  friend ostream& operator<<(ostream& os, Events& events);
};

#endif // EVENTS_H
