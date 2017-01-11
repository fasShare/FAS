#include <iostream>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "Epoll.h"

using namespace std;

Epoll::Epoll():
               epfd(-1) {
  //FIXME : EPOLL_CLOEXEC
  epfd = epoll_create(1);
  if(epfd == -1) {
    cout << "epoll_create error:" << strerror(errno) << endl;
  }
}

bool Epoll::event_ctl(int op, int fd, Epoll_Event* event) {
  assert(epfd != -1);
  int ret = epoll_ctl(epfd, op, fd, event);
  return ret == 0 ? true : false;
}

bool Epoll::event_add(Socket fd, Epoll_Event* event) {
  return event_add(fd.get_sd(), event);
}

bool Epoll::event_add(int fd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_ADD, fd, event);
}

bool Epoll::event_del(Socket fd, Epoll_Event* event) {
  return event_del(fd.get_sd(), event);
}

bool Epoll::event_del(int fd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_DEL, fd, event);
}

bool Epoll::event_mod(Socket fd, Epoll_Event* event) {
  return event_mod(fd.get_sd(), event);
}

bool Epoll::event_mod(int fd, Epoll_Event* event) {
  return event_ctl(EPOLL_CTL_MOD, fd, event);
}

int Epoll::loop_wait(Epoll_Event* events, int maxevents, int timeout) {
  //FIXME : The call was interrupted by a signal
  return epoll_wait(epfd, events, maxevents, timeout);
}

bool Epoll::Poller_event_add(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_add(event.data.fd, &event);
}

bool Epoll::Poller_event_mod(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_mod(event.data.fd, &event);
}

bool Epoll::Poller_event_del(Events* events) {
  Epoll_Event event = events->get_epollevents();
  return this->event_del(event.data.fd, &event);
}

int Epoll::Poller_loop(vector<Events> &events, int max_events, int timeout) {

  revents.resize(max_events);

  int ret = this->loop_wait(revents.data(), max_events, timeout);

  cout << "ret = " << ret << endl;
  cout << strerror(errno) << endl;

  for(int i = 0; i < ret; i++) {
    events.push_back(revents.at(i));
  }
  revents.clear();
  return ret;
}

Epoll::~Epoll() {
  close(epfd);
}

