#include <HttpReqHandle.h>
#include <HttpRequest.h>
#include <utilstring.h>
#include <utilfile.h>
#include <HttpCommon.h>
#include <TcpConnection.h>
#include <Log.h>


#include <boost/core/ignore_unused.hpp>

fas::http::ContentTypes globalContentType;

fas::http::HttpReqHandle::HttpReqHandle() {

}

void fas::http::HttpReqHandle::OnMessageCallback(TcpConnection *conn,
                                                 Buffer* buffer,
                                                 Timestamp time) {
  LOGGER_DEBUG << "fas::http::HttpReqHandle::OnMessageCallback" << fas::Log::CLRF;
  if (!request_.analyseHttpRequestHeader(buffer)) {
    return;
  }
  assert(request_.getReqestState() == fas::http::HttpRequest::ReqState::GOOD);

  this->handleMethod(conn, request_);
}

bool fas::http::HttpReqHandle::handleMethod(TcpConnection *conn, const HttpRequest& req) {
  const std::string method = req.getMethod();

  if (method == fas::http::Method::GET) {
    return this->HandleGet(conn, req);
  } else if (method == fas::http::Method::PUT) {
    return this->HandlePut(conn, req);
  } else if (method == fas::http::Method::HEAD) {
    return this->HandleHead(conn, req);
  } else if (method == fas::http::Method::DELETE) {
    return this->HandleDelete(conn, req);
  } else if (method == fas::http::Method::POST) {
    return this->HandlePost(conn, req);
  } else if (method == fas::http::Method::OPTIONS) {
    return this->HandleOptions(conn, req);
  } else if (method == fas::http::Method::TRACE) {
    return this->HandleTrace(conn, req);
  }
  return false;
}

bool fas::http::HttpReqHandle::HandleGet(TcpConnection *conn, const HttpRequest& req) {
  if (req.getPath().find("../") != std::string::npos) {
    this->HandleError(conn, req, "400");
    return false;
  }
  std::string file = options_.getServerPath() + req.getPath();
  struct stat st;
  fas::utils::GetFileStat(file, &st);
  if (fas::utils::IsRegularFile(&st)) {
    conn->sendString(req.getVersion() + " 200 OK\r\n");

    conn->sendString("Content-Length : " +
                             std::to_string(fas::utils::FileSizeInBytes(&st)));
    conn->sendString("Content-Type : " +
                             globalContentType.getType(fas::utils::StringGetSuffix(file)));
    conn->sendString("Connection : " + "close");
  }
  return true;
}

bool fas::http::HttpReqHandle::HandlePut(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandlePost(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleTrace(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleHead(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleDelete(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleOptions(TcpConnection *conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}


bool fas::http::HttpReqHandle::HandleError(TcpConnection *conn,
                                           const HttpRequest& req,
                                           const std::string& errorCode) {
  // FIXME : May be unreasonable.
  conn->shutdown();
}

fas::http::HttpReqHandle::~HttpReqHandle() {
  LOGGER_TRACE << "HttpReqHandle destroyed!" << fas::Log::CLRF;
}
