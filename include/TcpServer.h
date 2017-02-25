#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <Handle.h>
#include <Socket.h>
#include <Default.h>
#include <Events.h>
#include <NetAddress.h>
#include <EventLoopThreadPool.h>
#include <TcpConnection.h>

#include <memory>


class TcpServer {
public:
  TcpServer(EventLoop *loop, const NetAddress& addr);
  ~TcpServer();

  EventLoop* getLoop() const;

  bool start();

  void handleReadEvent(Events event, Timestamp time);

  void setMessageCallback(const MessageCallback& cb);

  void removeConnection(TcpConnShreadPtr conn);
  void removeConnectionInLoop(TcpConnShreadPtr conn);
private:
  Socket_t server_;
  EventLoop *loop_;
  Events events_;
  Handle *handle_;
  NetAddress addr_;
  const uint listenBacklog_;
  EventLoopThreadPool threadPool_;
  map<int, std::shared_ptr<TcpConnection>> conns_;

  MessageCallback messageCb_;
};

template<typename T>
shared_ptr<T> getSharedPtr(T *ptr) {
  return std::shared_ptr<T>(ptr);
}

#endif // FAS_TCPSERVER_H
