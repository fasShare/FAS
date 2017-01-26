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
int Events::getFd() {
  return fd_;
}

void Events::setFd(int fd) {
  this->fd_ = fd;
}

int Events::getEvents() {
  return events_;
}

void Events::setEvents(int events) {
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
  os << "fd = " << events.getFd();
  return os;
}
