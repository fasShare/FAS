#include "Handle.h"
#include <assert.h>

Handle::Handle(Events event) :
    event_(event),
    state_(EXECUTOR_STATE_ADD),
    set_event_flag_(true) {
}
Handle::Handle() :
    state_(EXECUTOR_STATE_ADD),
    set_event_flag_(false) {
}


 Events Handle::get_event() {
  assert(set_event_flag_);
  return event_;
}
 void Handle::set_event(Events& event) {
  if (set_event_flag_ == false) {
    event_ = event;
  }
  set_event_flag_ = true;
}


Events* Handle::get_eventpointer() {
  assert(set_event_flag_);
  return &event_;
}


void Handle::set_handle_read_event(const function<void \
                                          (Events*, Timestamp)>& handle_read_event) {
  handle_read_event_ = handle_read_event;
}

void Handle::set_handle_write_event(const function<void \
                                    (Events*, Timestamp)>& handle_write_event) {
  handle_write_event_ = handle_write_event;
}

void Handle::set_handle_error_event(const function<void \
                                    (Events*, Timestamp)>& handle_error_event) {
  handle_error_event_ = handle_error_event;
}

void Handle::set_handle_close_event(const function<void \
                                    (Events*, Timestamp)>& handle_close_event) {
  handle_close_event_ = handle_close_event;
}


void Handle::set_state(unsigned char state) {
  //FIXME:
  assert(true);
  state_ = state;
}
unsigned char Handle::get_state() {
  return state_;
}

void Handle::handleEvent(Events* events, Timestamp time) {
  if ((events->containsAtLeastOneEvents(POLLHUP)) && \
          !(events->containsAtLeastOneEvents(POLLIN))) {
     if (handle_close_event_) handle_close_event_(events, time);
   }

  if (events->containsAtLeastOneEvents(POLLNVAL)) {
  //logging << "Channel::handle_event() POLLNVAL";
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
  state_ = EXECUTOR_STATE_DEL;
  set_event_flag_ = false;
}
