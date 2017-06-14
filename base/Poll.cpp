#include <Poll.h>
#include <Timestamp.h>
#include <Events.h>
#include <Log.h>

size_t fas::Poll::kMaxPollNum_ = 1024;

fas::Poll::Poll() :
 revents_() {
}

bool fas::Poll::EventsAdd(Events* events) {
  assert(events != nullptr);
  if (revents_.size() >= kMaxPollNum_) {
    return false;
  }
  revents_.emplace_back(events->pollEvents());
  return true;
}

bool fas::Poll::EventsMod(Events* events) {
  assert(events != nullptr);
  auto pos = getEventPos(events->pollEvents());
  if (pos == revents_.end()) {
    return false;
  }
  (*pos) = events->pollEvents();
  return true;
}

bool fas::Poll::EventsDel(Events* events) {
  assert(events != nullptr);
  auto pos = getEventPos(events->pollEvents());
  if (pos == revents_.end()) {
    return false;
  }
  revents_.erase(pos);
  return true;
}

std::vector<struct pollfd>::iterator fas::Poll::getEventPos(const struct pollfd& event) {
  for (auto iter = revents_.begin(); iter != revents_.end(); ++iter) {
    if (iter->fd == event.fd) {
      return iter;
    }
  }
  return revents_.end();
}

fas::Timestamp fas::Poll::Loop(std::vector<fas::Events> &events, int timeout) {
should_continue:
  // FIXME : use ppoll
  int ret = ::poll(revents_.data(), revents_.size(), timeout);
  if (ret == -1) {
    if (errno == EINTR) {
      goto should_continue;
    }
    LOGGER_SYSERR << "poll error : " << ::strerror(errno) << Log::CLRF;
    events.clear();
    revents_.clear();
    return Timestamp::now();
  }
  int count = 0;
  for (auto iter = revents_.begin(); iter != revents_.end(); ++iter) {
    if (iter->revents != 0) {
      ++count;
      events.emplace_back(*iter);
      iter->revents = 0;
      if (count == ret) {
        break;
      }
    }
  }
  return Timestamp::now();
}
