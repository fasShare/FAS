#ifndef FAS_HTTPSERVER_H
#define FAS_HTTPSERVER_H
#include <map>
#include <memory>

#include <HttpReqHandle.h>
#include <Types.h>

namespace fas {

class EventLoop;
class TcpServer;
class NetAddress;

//using fas::map_conn_key_t;
//using fas::TcpConnShreadPtr;

namespace http {

class HttpServer {
public:
  HttpServer(EventLoop *loop, const NetAddress& addr);

  void OnNewConnection(fas::TcpConnShreadPtr conn);
  void OnConnectionRemoved(fas::map_conn_key_t conn);

  bool start();
private:
  EventLoop *loop_;
  TcpServer *tcpSer_;
  std::map<map_conn_key_t, std::shared_ptr<HttpReqHandle>> reqHandles_;
};

}

}

#endif //FAS_HTTPSERVER_H
