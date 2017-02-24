#include <iostream>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <Epoll.h>
#include <Log.h>

using namespace std;

Epoll::Epoll() :
    epoll_fd_(-1) {
  //FIXME : EPOLL_CLOEXEC
  epoll_fd_ = ::epoll_create(1);
  if(epoll_fd_ == -1) {
    LOG_SYSERR((string("ERROR epoll_ctl : ") + strerror(errno)));
  }
}

bool Epoll::eventCtl(int op, Socket_t sd, EpollEvent* event) {
  assert(epoll_fd_ != -1);
  int ret = ::epoll_ctl(epoll_fd_, op, sd, event);
  if (ret == -1) {
    LOG_SYSERR((string("ERROR epoll_ctl : ") + strerror(errno)));
    return false;
  } else {
    return true;
  }
}

bool Epoll::eventAdd(Socket_t sd, EpollEvent* event) {
  return eventCtl(EPOLL_CTL_ADD, sd, event);
}

bool Epoll::eventDel(Socket_t sd, EpollEvent* event) {
  return eventCtl(EPOLL_CTL_DEL, sd, event);
}

bool Epoll::eventMod(Socket_t sd, EpollEvent* event) {
  return eventCtl(EPOLL_CTL_MOD, sd, event);
}

int Epoll::loopWait(EpollEvent* events, int maxevents, int timeout) {
  //FIXME : The call was interrupted by a signal
should_continue:
  int ret = ::epoll_wait(epoll_fd_, events, maxevents, timeout);
  if (ret == -1) {
    if (errno == EINTR) {
      LOG_SYSERR((string("ERROR epoll_wait : ") + strerror(errno)));
      goto should_continue;
    }
    LOG_SYSERR((string("ERROR epoll_wait : ") + strerror(errno)));
    return false;
  } else {
    return ret;
  }
}

bool Epoll::pollerEventsAdd(Events* events) {
  EpollEvent event = events->epollEvents();
  return this->eventAdd(event.data.fd, &event);
}

bool Epoll::pollerEventsMod(Events* events) {
  EpollEvent event = events->epollEvents();
  return this->eventMod(event.data.fd, &event);
}

bool Epoll::pollerEventsDel(Events* events) {
  EpollEvent event = events->epollEvents();
  return this->eventDel(event.data.fd, &event);
}

int Epoll::pollerLoop(vector<Events> &events, int max_events, int timeout) {
  revents_.resize(max_events);

  assert(max_events > 0);
  int ret = this->loopWait(revents_.data(), max_events, timeout);

  for(int i = 0; i < ret; i++) {
    events.push_back(revents_.data()[i]);
  }

  //no use!
  revents_.clear();

  return ret;
}

Epoll::~Epoll() {
  ::close(epoll_fd_);
}

