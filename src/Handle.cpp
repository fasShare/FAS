#include <assert.h>


#include <Handle.h>
#include <Events.h>
#include <Timestamp.h>
#include <Log.h>


#include <boost/bind.hpp>

Handle::Handle(EventLoop* loop,
               Events event) :
  loop_(loop),
  event_(new Events(event)),
  state_(STATE_NEW) {
  checkRead_ = boost::bind(&Handle::defaultCheckRead, this, _1);
  checkWrite_ = boost::bind(&Handle::defaultCheckWrite, this, _1);
  checkErr_ = boost::bind(&Handle::defaultCheckError, this, _1);
  checkClose_ =  boost::bind(&Handle::defaultCheckClose, this, _1);
}

EventLoop* Handle::getLoop() {
  assert(loop_ != NULL);
  return loop_;
}

int Handle::fd() const {
  return event_->getFd();
}

Events* Handle::getEvent() const {
  return event_;
}



void Handle::setHandleRead(const events_handle_t& handle_read) {
  handle_read_event_ = handle_read;
}

void Handle::setHandleWrite(const events_handle_t& handle_write) {
  handle_write_event_ = handle_write;
}

void Handle::setHandleError(const events_handle_t& handle_error) {
  handle_error_event_ = handle_error;
}

void Handle::setHandleClose(const events_handle_t& handle_close) {
  handle_close_event_ = handle_close;
}


void Handle::setState(uchar state) {
  state_ = state;
}
uchar Handle::getState() {
  return state_;
}

void Handle::handleEvent(Events events, Timestamp time) {
  if (checkClose_(events)) {
     if (handle_close_event_) handle_close_event_(events, time);
  }

  if (checkErr_(events)) {
    if (handle_error_event_)
      handle_error_event_(events, time);
  }

  if (checkRead_(events)) {
    if (handle_read_event_)
      handle_read_event_(events, time);
  }

  if (checkWrite_(events)) {
    if (handle_write_event_)
      handle_write_event_(events, time);
  }

  if (events.containsAtLeastOneEvents(POLLNVAL)) {
    LOG_DEBUG("events contains POLLNVAL!");
  }
}

void Handle::setCheckRead(const EventCheckFunc& checkRead) {
  checkRead_ = checkRead;
}

void Handle::setCheckWrite(const EventCheckFunc& checkWrite) {
  checkWrite_ = checkWrite;
}

void Handle::setCheckError(const EventCheckFunc& checkErr) {
  checkErr_ = checkErr;
}

void Handle::setCheckClose(const EventCheckFunc& checkClose) {
  checkClose_ = checkClose;
}

bool Handle::defaultCheckRead(const Events& event) {
  return event.containsAtLeastOneEvents(POLLIN | POLLPRI | POLLRDHUP);
}

bool Handle::defaultCheckWrite(const Events& event) {
  return event.containsAtLeastOneEvents(POLLOUT);
}

bool Handle::defaultCheckError(const Events& event) {
  return event.containsAtLeastOneEvents(POLLERR | POLLNVAL);
}

bool Handle::defaultCheckClose(const Events& event) {
  return (event.containsAtLeastOneEvents(POLLHUP)) && \
          !(event.containsAtLeastOneEvents(POLLIN));
}


Handle::~Handle() {
  state_ = STATE_DEL;
  ::close(fd());
  // FIXME : use Log
  std::cout << "handle Destroyed!" << std::endl;
}
