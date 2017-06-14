#ifndef FAS_EVENTSPOLLER_H
#define FAS_EVENTSPOLLER_H
#include <vector>


#include <Timestamp.h>
#include <Events.h>


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace fas {
/*!
 * \brief The Poller class
 * The Poller class difine the common interface between EventLoop and Epoll or Poll class
 * use those interface, we can add, del and mol IO events wait on epoll or poll.
 */
class Poller : boost::noncopyable {
public:
  Poller();
  virtual bool EventsAdd(Events* events) = 0;
  virtual bool EventsMod(Events* events) = 0;
  virtual bool EventsDel(Events* events) = 0;
  virtual Timestamp Loop(std::vector<Events> &events, int timeout) = 0;
};

#define DEFAULT_POLLER Epoll

}
#endif // FAS_EVENTSPOLLER_H
