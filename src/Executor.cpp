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

void Executor::Executor_set_dispatcher(Dispatcher *dispatcher) {
  assert(set_event_flag);
  this->dispatch = dispatcher;
}
Dispatcher* Executor::Executor_get_dispatcher() {
  assert(set_event_flag);
  return dispatch;
}


void Executor::Executor_set_state(unsigned char state) {
  //FIXME:
  assert(true);
  this->state = state;
}
unsigned char Executor::Executor_get_state() {
  return this->state;
}


Executor::~Executor() {
  state = EXECUTOR_STATE_DEL;
  set_event_flag = false;
  dispatch = NULL;
}
