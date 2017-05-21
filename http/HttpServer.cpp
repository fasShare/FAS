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
  tcpSer_(new fas::TcpServer(loop_, addr, 0)) {

  tcpSer_->setOnConnectionCallBack(boost::bind(&HttpServer::OnNewConnection, this, _1));
  tcpSer_->setOnConnRemovedCallBack(boost::bind(&HttpServer::OnConnectionRemoved, this, _1));
}

void fas::http::HttpServer::OnNewConnection(fas::TcpConnShreadPtr conn) {
  LOGGER_TRACE << "fas::http::HttpServer::OnNewConnection" << fas::Log::CLRF;
  // req must not be shared_ptr, or conn will be referenced by itself.
  // you can analyse it by youself seriously.
  std::shared_ptr<HttpReqHandle> reqHandle = std::make_shared<HttpReqHandle>();

  conn->setOnMessageCallBack(boost::bind(&HttpReqHandle::OnMessageCallback, reqHandle, _1, _2, _3));
  conn->SetHasMoreDataCallback(boost::bind(&HttpReqHandle::sendMassData, reqHandle, _1));
  this->reqHandles_[connkey_t(conn->getConnfd(), conn.get())] = reqHandle;
}

void fas::http::HttpServer::OnConnectionRemoved(fas::connkey_t key) {
  LOGGER_TRACE << "fas::http::HttpServer::OnConnectionRemoved" << fas::Log::CLRF;
  loop_->assertInOwnerThread();
  if (this->reqHandles_.find(key) == this->reqHandles_.end()) {
    return;
  }

  int ret = this->reqHandles_.erase(key);
  assert(ret == 1);
  LOGGER_TRACE << "out of fas::http::HttpServer::OnConnectionRemoved" << fas::Log::CLRF;
}

bool fas::http::HttpServer::start() {
  assert(tcpSer_);
  return tcpSer_->start();
}
