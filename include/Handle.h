#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <iostream>
#include <boost/function.hpp>

#include <Types.h>
#include <Timestamp.h>
#include <Events.h>

#define EXECUTOR_STATE_ADD 1
#define EXECUTOR_STATE_MOD 2
#define EXECUTOR_STATE_DEL 3

class Dispatcher;
using namespace std;

class Handle {
private:
  Events event_;
  unsigned char state_;
  bool set_event_flag_;
  function<void (Events*, Timestamp)> handle_read_event_;
  function<void (Events*, Timestamp)> handle_write_event_;
  function<void (Events*, Timestamp)> handle_error_event_;
  function<void (Events*, Timestamp)> handle_close_event_;

public:
  typedef function<void (Events*, Timestamp)> events_handle_t;

  Handle(Events event);
  Handle();
  virtual ~Handle();

  Events get_event();
  void set_event(Events& event);

  Events* get_eventpointer();

  void set_handle_read(const events_handle_t& handle_read);
  void set_handle_write(const events_handle_t& handle_write);
  void set_handle_error(const events_handle_t& handle_error);
  void set_handle_close(const events_handle_t& handle_close);

  void handleEvent(Events*, Timestamp);

  void set_state(unsigned char state);
  unsigned char get_state();
};

#endif // EXECUTOR_H
