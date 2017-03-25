#include <Events.h>

fas::Events::Events(const int fd, uint32_t events) :
  fd_(fd),
  events_(events) {
}

fas::Events::Events(const Events& events) :
  Events(events.fd_, events.events_) {
}

fas::Events::Events(const EpollEvent& events) :
  Events(events.data.fd, events.events){
}

fas::Events::Events(const PollEvent& events) :
 Events(events.fd, events.revents) {

}

int fas::Events::getFd() const{
  return fd_;
}

uint32_t fas::Events::getEvents() const{
  return events_;
}

void fas::Events::updateEvents(uint32_t events) {
  events_ = events;
}

void fas::Events::addEvent(uint32_t event) {
  events_ |= event;
}

void fas::Events::deleteEvent(uint32_t event) {
  events_ &= ~event;
}

bool fas::Events::containsEvents(uint32_t events) const {
  return ((events_ & events) == events);
}

bool fas::Events::containsAtLeastOneEvents(uint32_t events) const {
  return ((events_ & events) != 0);
}

fas::EpollEvent fas::Events::epollEvents() {
  fas::EpollEvent events;
  bzero(&events, sizeof(EpollEvent));
  events.data.fd = this->getFd();
  events.events = this->getEvents();
  return events;
}

fas::PollEvent fas::Events::pollEvents() {
  fas::PollEvent events;
  bzero(&events, sizeof(PollEvent));
  events.fd = this->getFd();
  events.events = this->getEvents();
  return events;
}

std::ostream& fas::operator<<(std::ostream& os, Events& events) {
  os << "fd = " << events.getFd();
  return os;
}

fas::Events::~Events() {
}
