#include "Events.h"

Events::Events(int fd, int events) :
  fd_(fd),
  events_(events) {
}

Events::Events(const Events& events) :
  Events(events.fd_, events.events_) {
}

Events::Events(const EpollEvent& events) :
  Events(events.data.fd, events.events){
}

int Events::getFd() const{
  return fd_;
}

int Events::getEvents() {
  return events_;
}

void Events::updateEvents(int events) {
  this->events_ = events;
}

bool Events::containsEvents(int events) {
  return ((events_ & events) == events);
}

bool Events::containsAtLeastOneEvents(int events) {
  return ((events_ & events) != 0);
}

EpollEvent Events::epollEvents() {
  EpollEvent events;
  events.data.fd = this->fd_;
  events.events = this->events_;
  return events;
}

ostream& operator<<(ostream& os, Events& events) {
  os << "fd = " << events.getFd();
  return os;
}

Events::~Events() {
}
