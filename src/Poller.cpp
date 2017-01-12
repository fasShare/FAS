#include <Poller.h>
#include <Epoll.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

Poller::Poller() {
  boost::shared_ptr<Epoll> poll(new Epoll);

  events_add_ = boost::bind(&Epoll::poller_events_add, poll, _1);
  events_mod_ = boost::bind(&Epoll::poller_events_mod, poll, _1);
  events_del_ = boost::bind(&Epoll::poller_events_del, poll, _1);
  loop_ = boost::bind(&Epoll::poller_loop, poll, _1, _2, _3);
  boost::ignore_unused(events_add_, events_mod_, events_del_, loop_);
}





