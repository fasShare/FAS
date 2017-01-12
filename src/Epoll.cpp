#include <iostream>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "Epoll.h"

using namespace std;

Epoll::Epoll():
               epoll_fd_(-1) {
  //FIXME : EPOLL_CLOEXEC
  epoll_fd_ = epoll_create(1);
  if(epoll_fd_ == -1) {
    cout << "epoll_create error:" << strerror(errno) << endl;
  }
}

bool Epoll::event_ctl(int op, Socket_t sd, Epoll_Event* event) {
  assert(epoll_fd_ != -1);
  int ret = epoll_ctl(epoll_fd_, op, sd, event);
  return ret == 0 ? true : false;
}

bool Epoll::event_add(Socket_t sd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_ADD, sd, event);
}

bool Epoll::event_del(Socket_t sd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_DEL, sd, event);
}

bool Epoll::event_mod(Socket_t sd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_MOD, sd, event);
}

int Epoll::loop_wait(Epoll_Event* events, int maxevents, int timeout) {
  //FIXME : The call was interrupted by a signal
  return epoll_wait(epoll_fd_, events, maxevents, timeout);
}

bool Epoll::poller_events_add(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_add(event.data.fd, &event);
}

bool Epoll::poller_events_mod(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_mod(event.data.fd, &event);
}

bool Epoll::poller_events_del(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_del(event.data.fd, &event);
}

int Epoll::poller_loop(vector<Events> &events, int max_events, int timeout) {

  revents_.resize(max_events);

  int ret = this->loop_wait(revents_.data(), max_events, timeout);

  cout << "ret = " << ret << endl;
  if (ret == -1)
		cout << strerror(errno) << endl;

  for(int i = 0; i < ret; i++) {
    events.push_back(revents_.at(i));
  }

  //no use!
  revents_.clear();

  return ret;
}

Epoll::~Epoll() {
  close(epoll_fd_);
}

