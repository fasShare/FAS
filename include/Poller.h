#ifndef EVENTSPOLLER_H
#define EVENTSPOLLER_H
#include <Epoll.h>
#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>


using std::vector;
class Events;

class Poller : boost::noncopyable {
public:
  Poller();
  boost::function<bool (Events* events)> events_add_;
  boost::function<bool (Events* events)> events_mod_;
  boost::function<bool (Events* events)> events_del_;
  boost::function<bool (vector<Events> &events, int max_events, int timeout)> loop_;
};

#define DEFAULT_POLLER Epoll

#endif // EVENTSPOLLER_H
