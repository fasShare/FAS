#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <iostream>
#include "NetBaseTypes.h"
#include "Dispatcher.h"
#include "Events.h"

#define EXECUTOR_STATE_ADD 1
#define EXECUTOR_STATE_MOD 2
#define EXECUTOR_STATE_DEL 3

class Dispatcher;
using namespace std;

class Executor
{
private:
  Events event;
  unsigned char state;
  bool set_event_flag;
  Dispatcher *dispatch;
  shared_ptr<Executor> shared_this;
public:
  Executor(Events event);
  Executor();
  virtual ~Executor();

  Events Executor_get_event();
  void Executor_set_event(Events& event);

  Events* Executor_get_eventpointer();

  virtual void Executor_handle_event(Events* event) = 0;

  void Executor_set_dispatcher(Dispatcher *dispatch);
  Dispatcher* Executor_get_dispatcher();

  void Executor_set_state(unsigned char state);
  unsigned char Executor_get_state();
};

#endif // EXECUTOR_H
