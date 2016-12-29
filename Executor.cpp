#include "Executor.h"
#include <assert.h>

Executor::Executor(Events event):
                    event(event),
                    dispatch(NULL) {
  state = EXECUTOR_STATE_ADD;
  set_event_flag = true;
}
Executor::Executor() {
  state = EXECUTOR_STATE_ADD;
  set_event_flag = false;
  dispatch = NULL;
}

Executor::~Executor() {

}
Events Executor::Executor_get_event() {
  assert(set_event_flag);
  return event;
}
void Executor::Executor_set_event(Events& event) {
  if (set_event_flag == false) {
    this->event = event;
  }
  set_event_flag = true;
}

Events* Executor::Executor_get_eventpointer() {
  assert(set_event_flag);
  return &event;
}

void Executor::Executor_handle_event(Events* event) {
  cout << "event fd:" << event->Events_get_fd() << endl;
}
unsigned char Executor::Executor_get_event_state() {
  assert(set_event_flag);
  return state;
}

void Executor::Executor_set_event_state(unsigned char state) {
  assert(set_event_flag);
  this->state = state;
}
void Executor::Executor_set_dispatcher(Dispatcher *dispatcher) {
  assert(set_event_flag);
  this->dispatch = dispatcher;
}
