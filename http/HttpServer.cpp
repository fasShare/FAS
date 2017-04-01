#include <HttpServer.h>
#include <NetAddress.h>
#include <TcpServer.h>
#include <HttpRequest.h>
#include <EventLoop.h>
#include <Log.h>


#include <boost/bind.hpp>

fas::http::HttpServer::HttpServer(fas::EventLoop *loop,
                            const fas::NetAddress& addr) :
  loop_(loop),
  tcpSer_(new fas::TcpServer(loop_, addr)) {

  tcpSer_->setOnConnectionCallBack(boost::bind(&HttpServer::OnNewConnection, this, _1));
  tcpSer_->setOnConnRemovedCallBack(boost::bind(&HttpServer::OnConnectionRemoved, this, _1));
}

void fas::http::HttpServer::OnNewConnection(fas::TcpConnShreadPtr conn) {
  // req must not be shared_ptr, or conn will be referenced by itself.
  // you can analyse it by youself seriously.
  std::shared_ptr<HttpReqHandle> reqHandle = std::make_shared<HttpReqHandle>();

  conn->setOnMessageCallBack(boost::bind(&HttpReqHandle::OnMessageCallback, reqHandle, _1, _2, _3));

  this->reqHandles_[conn->getConnfd()] = std::shared_ptr<HttpReqHandle>(reqHandle);
}

void fas::http::HttpServer::OnConnectionRemoved(fas::map_conn_key_t key) {
  LOGGER_TRACE << "fas::http::HttpServer::OnConnectionRemoved" << fas::Log::CLRF;
  loop_->assertInOwnerThread();
  int ret = this->reqHandles_.erase(key);
  assert(ret == 1);
}

bool fas::http::HttpServer::start() {
  assert(tcpSer_);
  return tcpSer_->start();
}
