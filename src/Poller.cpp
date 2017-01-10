#include "Poller.h"
#include "Epoll.h"

Poller::Poller() {
}
Poller::~Poller() {
}
Poller* Poller::Poller_create() {
  return new Epoll();
}



