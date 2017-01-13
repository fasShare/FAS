#include "Events.h"

Events::Events(int fd, int events) :
    fd_(fd),
    events_(events){
}
Events::Events(const Epoll_Event& events) {
  fd_ = events.data.fd;
  events_ = events.events;
}

Events::~Events() {
}
int Events::get_fd() {
  return fd_;
}

void Events::set_fd(int fd) {
  this->fd_ = fd;
}

int Events::get_events() {
  return events_;
}

void Events::set_events(int events) {
  this->events_ = events;
}

bool Events::containsEvents(int events) {
  return ((events_ & events) == events);
}

bool Events::containsAtLeastOneEvents(int events) {
  return ((events_ & events) != 0);
}

Epoll_Event Events::getEpollevents() {
  Epoll_Event events;
  events.data.fd = this->fd_;
  events.events = this->events_;
  return events;
}

ostream& operator<<(ostream& os, Events& events) {
  os << "fd = " << events.get_fd();
  return os;
}
