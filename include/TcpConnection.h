#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <Default.h>
#include <Buffer.h>
#include <Events.h>
#include <Handle.h>
#include <EventLoop.h>

class TcpConnection {
public:
  TcpConnection(EventLoop *loop, const Events& event);

  EventLoop* getLoop();

  void setHandleRead(const events_handle_t& handle_read);
  void setHandleWrite(const events_handle_t& handle_write);
  void setHandleError(const events_handle_t& handle_error);
  void setHandleClose(const events_handle_t& handle_close);

  void defaultHandleRead(Events* revents, Timestamp time);
  void defaultHandleWrite(Events* revents, Timestamp time);
  void defaultHandleError(Events* revents, Timestamp time);
  void defaultHandleClose(Events* revents, Timestamp time);

private:
  EventLoop *loop_;
  Events event_;
  Handle *handle_;
  Buffer buffer_;
};

#endif // FAS_TCPCONNECTION_H

