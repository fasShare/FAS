#include <Handle.h>
#include <assert.h>
#include <Log.h>

Handle::Handle(Events event) :
    event_(event),
    state_(STATE_ADD),
    set_event_flag_(true) {
}
Handle::Handle() :
    state_(STATE_ADD),
    set_event_flag_(false) {
}

Dispatcher* Handle::loop() {
	assert(loop_ != NULL);
	return loop_;
}
void Handle::setLoop(Dispatcher* loop) {
    assert((loop != NULL) && (loop_ == NULL));
	loop_ = loop;
}

Events Handle::getEvent() {
  assert(set_event_flag_);
  return event_;
}
 void Handle::setEvent(Events& event) {
  if (set_event_flag_ == false) {
    event_ = event;
  }
  set_event_flag_ = true;
}


Events* Handle::getEventPtr() {
  assert(set_event_flag_);
  return &event_;
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


void Handle::setState(unsigned char state) {
  assert(true);
  state_ = state;
}
unsigned char Handle::getState() {
  return state_;
}

void Handle::handleEvent(Events* events, Timestamp time) {
  if ((events->containsAtLeastOneEvents(POLLHUP)) && \
          !(events->containsAtLeastOneEvents(POLLIN))) {
     if (handle_close_event_) handle_close_event_(events, time);
   }

  if (events->containsAtLeastOneEvents(POLLNVAL)) {
    LOG_DEBUG("events contains POLLNVAL!");
  }

  if (events->containsAtLeastOneEvents(POLLERR | POLLNVAL)) {
    if (handle_error_event_)
      handle_error_event_(events, time);

  }
  if (events->containsAtLeastOneEvents(POLLIN | POLLPRI | POLLRDHUP)) {
    if (handle_read_event_)
      handle_read_event_(events, time);

  }
  if (events->containsAtLeastOneEvents(POLLOUT)) {
    if (handle_write_event_)
      handle_write_event_(events, time);
  }
}

Handle::~Handle() {
  state_ = STATE_DEL;
  set_event_flag_ = false;
}
