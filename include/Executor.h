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
public:
  Executor(Events event);
  Executor();
  virtual ~Executor();

  Events Executor_get_event();
  void Executor_set_event(Events& event);

  Events* Executor_get_eventpointer();

  virtual void Executor_handle_event(Events* event);

  unsigned char Executor_get_event_state();
  void Executor_set_event_state(unsigned char set_event);

  void Executor_set_dispatcher(Dispatcher *dispatch);

  void Executor_set_state(unsigned char state);
  unsigned char Executor_get_state();
};

#endif // EXECUTOR_H
