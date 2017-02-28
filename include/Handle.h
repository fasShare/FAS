#ifndef FAS_EXECUTOR_H
#define FAS_EXECUTOR_H
#include <iostream>


#include <Types.h>


#include <boost/function.hpp>

class EventLoop;
class Events;
class Timestamp;

class Handle {
public:
  enum state {
    STATE_ADD = 1,
    STATE_MOD,
    STATE_DEL,
    STATE_LOOP,
    STATE_NEW,
  };

  Handle(EventLoop* loop, Events event);
  virtual ~Handle();

  EventLoop* getLoop();
  
  int fd() const;
  Events* getEvent() const;
  void updateEvent(Events& event);

  void setState(uchar state);
  uchar getState();

  void setCheckRead(const EventCheckFunc& checkRead);
  void setCheckWrite(const EventCheckFunc& checkWrite);
  void setCheckError(const EventCheckFunc& checkErr);
  void setCheckClose(const EventCheckFunc& checkClose);

  bool defaultCheckRead(const Events& event);
  bool defaultCheckWrite(const Events& event);
  bool defaultCheckError(const Events& event);
  bool defaultCheckClose(const Events& event);

  void setHandleRead(const events_handle_t& handle_read);
  void setHandleWrite(const events_handle_t& handle_write);
  void setHandleError(const events_handle_t& handle_error);
  void setHandleClose(const events_handle_t& handle_close);

  void handleEvent(Events, Timestamp);
private:
  EventLoop *loop_;
  Events *event_;
  uint state_;

  EventCheckFunc checkRead_;
  EventCheckFunc checkWrite_;
  EventCheckFunc checkClose_;
  EventCheckFunc checkErr_;

  boost::function<void (Events, Timestamp)> handle_read_event_;
  boost::function<void (Events, Timestamp)> handle_write_event_;
  boost::function<void (Events, Timestamp)> handle_error_event_;
  boost::function<void (Events, Timestamp)> handle_close_event_;
};

#endif // FAS_EXECUTOR_H
