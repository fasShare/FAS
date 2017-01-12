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


void Handle::set_handle_event(const function<void (Events*, Timestamp)>& handle_event) {
  handle_event_ = handle_event;
}


void Handle::set_state(unsigned char state) {
  //FIXME:
  assert(true);
  state_ = state;
}
unsigned char Handle::get_state() {
  return state_;
}


Handle::~Handle() {
  state_ = EXECUTOR_STATE_DEL;
  set_event_flag_ = false;
}
