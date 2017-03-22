#include <Events.h>

using fas::Events;

Events::Events(const int fd, uint32_t events) :
  fd_(fd),
  events_(events) {
}

Events::Events(const Events& events) :
  Events(events.fd_, events.events_) {
}

Events::Events(const EpollEvent& events) :
  Events(events.data.fd, events.events){
}

Events::Events(const PollEvent& events) :
 Events(events.fd, events.revents) {

}

int Events::getFd() const{
  return fd_;
}

uint32_t Events::getEvents() const{
  return events_;
}

void Events::updateEvents(uint32_t events) {
  events_ = events;
}

void Events::addEvent(uint32_t event) {
  events_ |= event;
}

void Events::deleteEvent(uint32_t event) {
  events_ &= ~event;
}

bool Events::containsEvents(uint32_t events) const {
  return ((events_ & events) == events);
}

bool Events::containsAtLeastOneEvents(uint32_t events) const {
  return ((events_ & events) != 0);
}

fas::EpollEvent Events::epollEvents() {
  fas::EpollEvent events;
  bzero(&events, sizeof(EpollEvent));
  events.data.fd = this->getFd();
  events.events = this->getEvents();
  return events;
}

fas::PollEvent Events::pollEvents() {
  fas::PollEvent events;
  bzero(&events, sizeof(PollEvent));
  events.fd = this->getFd();
  events.events = this->getEvents();
  return events;
}

std::ostream& operator<<(std::ostream& os, Events& events) {
  os << "fd = " << events.getFd();
  return os;
}

Events::~Events() {
}
