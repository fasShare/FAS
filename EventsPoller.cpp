#include "EventsPoller.h"
#include "Epoll.h"

EventsPoller::EventsPoller() {
}
EventsPoller::~EventsPoller() {
}
EventsPoller* EventsPoller::Poller_create() {
  return new Epoll();
}



