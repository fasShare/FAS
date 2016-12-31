#include "Events.h"
Events::Events(int fd, int events):
                fd(fd),
                events(events){
}
Events::Events(const Epoll_Event& events) {
  this->fd = events.data.fd;
  this->events = events.events;
}

Events::~Events() {
}
int Events::Events_get_fd() {
  return fd;
}

int Events::Events_set_fd(int fd) {
  this->fd = fd;
}

int Events::Events_get_events() {
  return events;
}

int Events::Events_set_events(int events) {
  this->events = events;
}

Epoll_Event Events::Events_get_epollevents() {
  Epoll_Event events;
  events.data.fd = this->fd;
  events.events = this->events;
  return events;
}

ostream& operator<<(ostream& os, Events& events) {
  os << "fd = " << events.fd << endl;
  return os;
}
