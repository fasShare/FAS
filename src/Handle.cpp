#include "Handle.h"
#include <assert.h>

Handle::Handle(Events event):
                    event(event) {
  state = EXECUTOR_STATE_ADD;
  set_event_flag = true;
}
Handle::Handle() {
  state = EXECUTOR_STATE_ADD;
  set_event_flag = false;
}


Events Handle::get_event() {
  assert(set_event_flag);
  return event;
}
void Handle::set_event(Events& event) {
  if (set_event_flag == false) {
    this->event = event;
  }
  set_event_flag = true;
}


Events* Handle::get_eventpointer() {
  assert(set_event_flag);
  return &event;
}


void Handle::set_handle_event(const function<void (Events*)>& handle_event) {
  this->handle_event = handle_event;
}


void Handle::set_state(unsigned char state) {
  //FIXME:
  assert(true);
  this->state = state;
}
unsigned char Handle::get_state() {
  return this->state;
}


Handle::~Handle() {
  state = EXECUTOR_STATE_DEL;
  set_event_flag = false;
}
