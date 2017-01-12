#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <iostream>
#include <boost/function.hpp>

#include <NetBaseTypes.h>
#include <Timestamp.h>
#include <Events.h>

#define EXECUTOR_STATE_ADD 1
#define EXECUTOR_STATE_MOD 2
#define EXECUTOR_STATE_DEL 3

class Dispatcher;
using namespace std;

class Handle
{
private:
  Events event_;
  unsigned char state_;
  bool set_event_flag_;

public:
  function<void (Events*, Timestamp)> handle_event_;

public:
  Handle(Events event);
  Handle();
  virtual ~Handle();

  Events get_event();
  void set_event(Events& event);

  Events* get_eventpointer();

  void set_handle_event(const function<void (Events*, Timestamp)>& handle_event);

  void set_state(unsigned char state);
  unsigned char get_state();
};

#endif // EXECUTOR_H
