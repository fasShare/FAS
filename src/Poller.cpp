

#include <Poller.h>
#include <Epoll.h>
#include <Poll.h>


#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

Poller::Poller() {
  boost::shared_ptr<DEFAULT_POLLER> poll(new DEFAULT_POLLER);

  events_add_ = boost::bind(&DEFAULT_POLLER::pollerEventsAdd, poll, _1);
  events_mod_ = boost::bind(&DEFAULT_POLLER::pollerEventsMod, poll, _1);
  events_del_ = boost::bind(&DEFAULT_POLLER::pollerEventsDel, poll, _1);
  loop_ = boost::bind(&DEFAULT_POLLER::pollerLoop, poll, _1, _2);
  boost::ignore_unused(events_add_, events_mod_, events_del_, loop_);
}





