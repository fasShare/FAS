#include <iostream>


#include <HttpReqHandle.h>
#include <HttpRequest.h>
#include <utilstring.h>
#include <utilfile.h>
#include <HttpCommon.h>
#include <TcpConnection.h>
#include <Log.h>


#include <boost/core/ignore_unused.hpp>

fas::http::ContentTypes globalContentType;

const size_t fas::http::HttpReqHandle::SendMassDataContext::readEveryTime_ = 1024;

fas::http::HttpReqHandle::HttpReqHandle() :
  handingMethod_(false) {

}

void fas::http::HttpReqHandle::OnMessageCallback(TcpConnection *conn,
                                                 Buffer* buffer,
                                                 Timestamp time) {
  LOGGER_DEBUG << "fas::http::HttpReqHandle::OnMessageCallback" << fas::Log::CLRF;
  if (!request_.analyseHttpRequestHeader(buffer)) {
    return;
  }

  std::cout << request_.getMethod() << " " << request_.getPath() << " " << request_.getVersion() << std::endl;

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
  std::string getpath = req.getPath();
  if ((getpath == "") || (getpath == "/")) {
    getpath = "index.html";
  }
  std::string file = options_.getServerPath() + getpath;
  std::cout << options_.getServerPath() << std::endl;
  std::cout << file << std::endl;
  for (auto iter : req.getHeaders()) {
    std::cout << iter.first << " : " << iter.second << std::endl;
  }

  struct stat st;
  if (!fas::utils::GetFileStat(file, &st)) {
    this->HandleError(conn, req, "400");
    return false;
  }
  if (fas::utils::IsRegularFile(&st)) {

    int fdret = open(file.c_str(), O_RDONLY);
    if (fdret < 0) {
      HandleError(conn, req, "400");
      return false;
    }

    massDataC_.fd_ = fdret;
    massDataC_.length_ = fas::utils::FileSizeInBytes(&st);
    massDataC_.rdstart_ = 0;
    massDataC_.remaind_ = massDataC_.length_;

    conn->sendString(req.getVersion() + " 200 OK\r\n");

    conn->sendString(std::string("Content-Length : ") +
                      std::to_string(fas::utils::FileSizeInBytes(&st)) + "\r\n");
    conn->sendString(std::string("Content-Type : ") +
                      globalContentType.getType(fas::utils::StringGetSuffix(file)) +
                     "\r\n");
    conn->sendString(std::string("Connection : ") + "close\r\n");
    conn->sendString("\r\n");

    conn->setHasMoreData();
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

void fas::http::HttpReqHandle::sendMassData(TcpConnection *conn) {
  assert(massDataC_.fd_ > 0);
  uchar buf[SendMassDataContext::readEveryTime_ + 1];
  int ret = read(massDataC_.fd_, buf, SendMassDataContext::readEveryTime_);
  if (ret == 0) {
    conn->unsetHasMoreData();
    massDataC_.fd_ = 0;
  } else if (ret < 0) {
    if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
      return;
    }
    conn->unsetHasMoreData();
    conn->shutdown();
    return;
  } else {
    conn->putDataToWriteBuffer(buf, ret);
    massDataC_.rdstart_ += ret;
    massDataC_.remaind_ -= ret;
    assert(massDataC_.length_ == (massDataC_.rdstart_ + massDataC_.remaind_));

    if (massDataC_.remaind_ == 0) {
      conn->unsetHasMoreData();
      massDataC_.fd_ = 0;
      conn->shutdown();
    }
  }

}

bool fas::http::HttpReqHandle::HandleError(TcpConnection *conn,
                                           const HttpRequest& req,
                                           const std::string& errorCode) {
  // FIXME : May be unreasonable.
  conn->shutdown();
  return false;
}

fas::http::HttpReqHandle::~HttpReqHandle() {
  LOGGER_TRACE << "HttpReqHandle destroyed!" << fas::Log::CLRF;
}
