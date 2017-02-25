#include <assert.h>


#include <Handle.h>
#include <Events.h>
#include <Timestamp.h>
#include <Log.h>


Handle::Handle(EventLoop* loop,
               Events event) :
  loop_(loop),
  event_(new Events(event)),
  state_(STATE_NEW) {

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
  if ((events.containsAtLeastOneEvents(POLLHUP)) && \
          !(events.containsAtLeastOneEvents(POLLIN))) {
     if (handle_close_event_) handle_close_event_(events, time);
   }

  if (events.containsAtLeastOneEvents(POLLNVAL)) {
    LOG_DEBUG("events contains POLLNVAL!");
  }

  if (events.containsAtLeastOneEvents(POLLERR | POLLNVAL)) {
    if (handle_error_event_)
      handle_error_event_(events, time);

  }
  if (events.containsAtLeastOneEvents(POLLIN | POLLPRI | POLLRDHUP)) {
    if (handle_read_event_)
      handle_read_event_(events, time);

  }
  if (events.containsAtLeastOneEvents(POLLOUT)) {
    if (handle_write_event_)
      handle_write_event_(events, time);
  }
}

Handle::~Handle() {
  state_ = STATE_DEL;
}
