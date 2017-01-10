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
int Events::get_fd() {
  return fd;
}

void Events::set_fd(int fd) {
  this->fd = fd;
}

int Events::get_events() {
  return events;
}

void Events::set_events(int events) {
  this->events = events;
}

Epoll_Event Events::get_epollevents() {
  Epoll_Event events;
  events.data.fd = this->fd;
  events.events = this->events;
  return events;
}

ostream& operator<<(ostream& os, Events& events) {
  os << "fd = " << events.fd << endl;
  return os;
}
