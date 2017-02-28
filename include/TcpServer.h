#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <memory>
#include <map>
#include <vector>


#include <Socket.h>
#include <Default.h>
#include <Events.h>
#include <TcpConnection.h>
#include <NetAddress.h>
#include <EventLoopThreadPool.h>

class EventLoop;
class Handle;

class TcpServer {
public:
  TcpServer(EventLoop *loop, const NetAddress& addr);
  ~TcpServer();

  EventLoop* getLoop() const;

  TcpConnShreadPtr getConn(map_conn_key_t key) const;
  TcpConnShreadPtr getConn(map_conn_key_t key);

  bool start();

  void handleReadEvent(Events event, Timestamp time);

  void setMessageCallback(const MessageCallback& cb);

  void removeConnection(map_conn_key_t conn);
  void removeConnectionInLoop(map_conn_key_t conn);
private:
  Socket server_;
  EventLoop *loop_;
  Events events_;
  Handle *handle_;
  NetAddress addr_;
  const uint listenBacklog_;
  EventLoopThreadPool threadPool_;
  std::map<int, std::shared_ptr<TcpConnection>> conns_;

  MessageCallback messageCb_;
};

#endif // FAS_TCPSERVER_H
