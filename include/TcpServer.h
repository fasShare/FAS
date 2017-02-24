#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <Handle.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Types.h>
#include <EventLoopThreadPool.h>

class TcpServer {
public:
    TcpServer(EventLoop *loop, const NetAddress& addr);
    ~TcpServer();

    EventLoop* getLoop() const;

    bool start();

    void handleReadEvent(Events* event, Timestamp time);
private:
  Socket_t server_;
  EventLoop *loop_;
  Events events_;
  Handle *handle_;
  NetAddress addr_;
  const uint listenBacklog_;
  EventLoopThreadPool threadPool_;
};

#endif // FAS_TCPSERVER_H
