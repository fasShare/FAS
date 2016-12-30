#include "Epoll.h"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

using namespace std;

Epoll::Epoll():
               epfd(-1) {
  //FIXME : EPOLL_CLOEXEC
  epfd = epoll_create(1);
  if(epfd == -1) {
    cout << "epoll_create error:" << strerror(errno) << endl;
  }
}

bool Epoll::Epoll_ctl(int op, int fd, Epoll_Event* event) {
  assert(epfd != -1);
  int ret = epoll_ctl(epfd, op, fd, event);
  return ret == 0 ? true : false;
}

bool Epoll::Epoll_event_add(Socket fd, Epoll_Event* event) {
  return Epoll_event_add(fd.Socket_get_sd(), event);
}

bool Epoll::Epoll_event_add(int fd, Epoll_Event* event) {
  return Epoll_ctl(EPOLL_CTL_ADD, fd, event);
}

bool Epoll::Epoll_event_del(Socket fd, Epoll_Event* event) {
  return Epoll_event_del(fd.Socket_get_sd(), event);
}

bool Epoll::Epoll_event_del(int fd, Epoll_Event* event) {
  return Epoll_ctl(EPOLL_CTL_DEL, fd, event);
}

bool Epoll::Epoll_event_mod(Socket fd, Epoll_Event* event) {
  return Epoll_event_mod(fd.Socket_get_sd(), event);
}

bool Epoll::Epoll_event_mod(int fd, Epoll_Event* event) {
  return Epoll_ctl(EPOLL_CTL_MOD, fd, event);
}

int Epoll::Epoll_wait(Epoll_Event* events, int maxevents, int timeout) {
  //FIXME : The call was interrupted by a signal
  return epoll_wait(epfd, events, maxevents, timeout);
}

bool Epoll::Poller_event_add(Events* events) {
  Epoll_Event event = events->Events_get_epollevents();
  return this->Epoll_event_add(event.data.fd, &event);
}

bool Epoll::Poller_event_mod(Events* events) {
  Epoll_Event event = events->Events_get_epollevents();
  return this->Epoll_event_mod(event.data.fd, &event);
}

bool Epoll::Poller_event_del(Events* events) {
  Epoll_Event event = events->Events_get_epollevents();
  return this->Epoll_event_del(event.data.fd, &event);
}

int Epoll::Poller_loop(vector<Events> &events, int max_events, int timeout) {
  if(revents.size() <= max_events) {
    //FIXME
    revents.resize(max_events + 20);
  }
  int ret = this->Epoll_wait(revents.data(), max_events, timeout);
  for(int i = 0; i < ret; i++){
    events.push_back(revents.at(i));
  }
  revents.clear();
  return ret;
}

Epoll::~Epoll() {
  close(epfd);
}

