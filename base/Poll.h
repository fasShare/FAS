#ifndef FAS_POLL_H
#define FAS_POLL_H
#include <sys/poll.h>


#include <Types.h>
#include <Poll.h>


#include <boost/noncopyable.hpp>

namespace fas {

class Poll : boost::noncopyable {
public:
  Poll();

  bool pollerEventsAdd(Events* events);
  bool pollerEventsMod(Events* events);
  bool pollerEventsDel(Events* events);
  Timestamp pollerLoop(std::vector<Events> &events, int timeout);

private:
  std::vector<PollEvent>::iterator getEventPos(const PollEvent& event);
  static size_t kMaxPollNum_;
  std::vector<PollEvent> revents_;
};

}
#endif // FAS_POLL_H

