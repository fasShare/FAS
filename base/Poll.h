#ifndef FAS_POLL_H
#define FAS_POLL_H
#include <sys/poll.h>


#include <Poller.h>


#include <boost/noncopyable.hpp>

namespace fas {

class Poll : public Poller {
public:
    Poll();

    bool EventsAdd(Events* events);
    bool EventsMod(Events* events);
    bool EventsDel(Events* events);
    Timestamp Loop(std::vector<Events> &events, int timeout);

private:
    std::vector<struct pollfd>::iterator getEventPos(const struct pollfd& event);
    static size_t kMaxPollNum_;
    std::vector<struct pollfd> revents_;
};

}
#endif // FAS_POLL_H

