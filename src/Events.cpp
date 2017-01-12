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

Epoll_Event Events::get_epollevents() {
  Epoll_Event events;
  events.data.fd = this->fd_;
  events.events = this->events_;
  return events;
}

ostream& operator<<(ostream& os, Events& events) {
  os << "fd = " << events.fd_ << endl;
  return os;
}
