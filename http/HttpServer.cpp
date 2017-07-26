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
}

void fas::http::HttpServer::OnNewConnection(fas::TcpServer::TcpConnShreadPtr conn) {
  LOGGER_TRACE("fas::http::HttpServer::OnNewConnection");
  // req must not be shared_ptr, or conn will be referenced by itself.
  // you can analyse it by youself seriously.
  std::shared_ptr<HttpReqHandle> reqHandle = std::make_shared<HttpReqHandle>();

  conn->setOnMessageCallBack(boost::bind(&HttpReqHandle::OnMessageCallback, reqHandle, _1, _2, _3));
  conn->SetHasMoreDataCallback(boost::bind(&HttpReqHandle::sendMassData, reqHandle, _1));
}

bool fas::http::HttpServer::start() {
  assert(tcpSer_);
  return tcpSer_->start();
}
