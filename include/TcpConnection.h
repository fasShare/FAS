#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <memory>


#include <Default.h>
#include <Buffer.h>
#include <Events.h>

class Handle;
class EventLoop;

class TcpConnection {
public:
  TcpConnection(EventLoop *loop, const Events& event);

  EventLoop* getLoop();

  int getConnfd() const;

  void closeAndClearTcpConnection();

  void setOnMessageCallBack(const MessageCallback& cb);
  void setOnCloseCallBack(const CloseCallback& cb);

  void handleRead(Events revents, Timestamp time);
  void handleWrite(Events revents, Timestamp time);
  void handleError(Events revents, Timestamp time);
  void handleClose(Events revents, Timestamp time);

private:
  EventLoop *loop_;
  Events event_;
  Handle *handle_;
  Buffer buffer_;
  int connfd_;

  bool closeing_;
  CloseCallback closeCb_;
  MessageCallback messageCb_;
};

#endif // FAS_TCPCONNECTION_H

