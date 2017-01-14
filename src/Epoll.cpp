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

bool Epoll::eventCtl(int op, Socket_t sd, Epoll_Event* event) {
  assert(epoll_fd_ != -1);
  int ret = epoll_ctl(epoll_fd_, op, sd, event);
  return ret == 0 ? true : false;
}

bool Epoll::eventAdd(Socket_t sd, Epoll_Event* event) {
  return eventCtl(EPOLL_CTL_ADD, sd, event);
}

bool Epoll::eventDel(Socket_t sd, Epoll_Event* event) {
  return eventCtl(EPOLL_CTL_DEL, sd, event);
}

bool Epoll::eventMod(Socket_t sd, Epoll_Event* event) {
  return eventCtl(EPOLL_CTL_MOD, sd, event);
}

int Epoll::loopWait(Epoll_Event* events, int maxevents, int timeout) {
  //FIXME : The call was interrupted by a signal
  return epoll_wait(epoll_fd_, events, maxevents, timeout);
}

bool Epoll::pollerEventsAdd(Events* events) {
  Epoll_Event event = events->getEpollevents();
  return this->eventAdd(event.data.fd, &event);
}

bool Epoll::pollerEventsMod(Events* events) {
  Epoll_Event event = events->getEpollevents();
  return this->eventMod(event.data.fd, &event);
}

bool Epoll::pollerEventsDel(Events* events) {
  Epoll_Event event = events->getEpollevents();
  return this->eventDel(event.data.fd, &event);
}

int Epoll::pollerLoop(vector<Events> &events, int max_events, int timeout) {
  revents_.resize(max_events);

  int ret = this->loopWait(revents_.data(), max_events, timeout);

  cout << "ret = " << ret << endl;
  cout << strerror(errno) << endl;

  for(int i = 0; i < ret; i++) {
    events.push_back(revents_.data()[i]);
  }

  //no use!
  revents_.clear();

  return ret;
}

Epoll::~Epoll() {
  close(epoll_fd_);
}

