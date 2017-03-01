#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <memory>


#include <Types.h>
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>

class Handle;
class EventLoop;
class buffer;

class TcpConnection {
public:
  TcpConnection(EventLoop *loop, const Events& event);
  ~TcpConnection();

  EventLoop* getLoop();

  Socket_t getConnfd() const;

  void setPeerAddr(const NetAddress& addr);

  void closeAndClearTcpConnection();

  void setOnMessageCallBack(const MessageCallback& cb);
  void setOnCloseCallBack(const CloseCallback& cb);

  size_t sendString(const std::string& msg);

  void handleRead(Events revents, Timestamp time);
  void handleWrite(Events revents, Timestamp time);
  void handleError(Events revents, Timestamp time);
  void handleClose(Events revents, Timestamp time);

private:
  EventLoop *loop_;
  Events event_;
  Handle *handle_;
  Buffer *readBuffer_;
  Buffer *writeBuffer_;
  Socket connfd_;
  NetAddress peerAddr_;
  bool closeing_;
  CloseCallback closeCb_;
  MessageCallback messageCb_;
};

void TcpConnMessageCallback(TcpConnection *conn,
                            Buffer *buffer,
                            Timestamp time);

#endif // FAS_TCPCONNECTION_H

