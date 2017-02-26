#include <Events.h>

Events::Events(int fd, uint32_t events) :
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

uint32_t Events::getEvents() const{
  return events_;
}

void Events::updateEvents(uint32_t events) {
  this->events_ = events;
}

bool Events::containsEvents(uint32_t events) const {
  return ((events_ & events) == events);
}

bool Events::containsAtLeastOneEvents(uint32_t events) const {
  return ((events_ & events) != 0);
}

EpollEvent Events::epollEvents() {
  EpollEvent events;
  events.data.fd = this->fd_;
  events.events = this->events_;
  return events;
}

std::ostream& operator<<(std::ostream& os, Events& events) {
  os << "fd = " << events.getFd();
  return os;
}

Events::~Events() {
}
