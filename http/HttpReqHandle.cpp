#include <iostream>


#include <HttpReqHandle.h>
#include <HttpRequest.h>
#include <utilstring.h>
#include <utilfile.h>
#include <Thread.h>
#include <HttpCommon.h>
#include <TcpConnection.h>
#include <Log.h>


#include <boost/core/ignore_unused.hpp>

fas::http::ContentTypes globalContentType;

const size_t fas::http::HttpReqHandle::SendMassDataContext::readEveryTime_ = 1024;

fas::http::HttpReqHandle::HttpReqHandle() :
  handingMethod_(false) {

}

void fas::http::HttpReqHandle::OnMessageCallback(TcpConnShreadPtr conn,
                                                 Buffer* buffer,
                                                 Timestamp time) {
  LOGGER_TRACE("tid : " << gettid() << " HttpReqHandle::OnMessageCallback");
  //如果首部字段存在错误
  if (!request_.analyseHttpRequestHeader(buffer)) {
    return;
  }

  std::cout << request_.getMethod() << " " << request_.getPath() << " " << request_.getVersion() << std::endl;

  assert(request_.getReqestState() == fas::http::HttpRequest::ReqState::GOOD);

  this->handleMethod(conn, request_);
}

bool fas::http::HttpReqHandle::handleMethod(TcpConnShreadPtr conn, const HttpRequest& req) {
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

bool fas::http::HttpReqHandle::HandleGet(TcpConnShreadPtr conn, const HttpRequest& req) {
  if (req.getPath().find("../") != std::string::npos) {
    this->HandleError(conn, req, "400");
    return false;
  }
  std::string getpath = req.getPath();
  if ((getpath == "") || (getpath == "/")) {
    getpath = "index.html";
  }

  std::string file = options_.getServerPath() + getpath;


  LOGGER_TRACE("tid : " << gettid() << " get file: " << file);
#if 1
  for (auto iter : req.getHeaders()) {
    LOGGER_TRACE(iter.first << " : " << iter.second);
  }
#endif
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

    massDataC_.ContextReset(fdret, fas::utils::FileSizeInBytes(&st), 0);

    conn->sendString(req.getVersion() + " 200 OK\r\n");

    LOGGER_TRACE("send file length : " << std::to_string(fas::utils::FileSizeInBytes(&st)));

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

bool fas::http::HttpReqHandle::HandlePut(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandlePost(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleTrace(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleHead(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleDelete(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

bool fas::http::HttpReqHandle::HandleOptions(TcpConnShreadPtr conn, const HttpRequest& req) {
  boost::ignore_unused(conn, req);
  return true;
}

void fas::http::HttpReqHandle::sendMassData(TcpConnShreadPtr conn) {
  assert(massDataC_.getFd() > 0);
  uint8_t buf[massDataC_.getReadEveryTime() + 1];

  if (lseek(massDataC_.getFd(), massDataC_.getRdstart(), SEEK_SET) == static_cast<off_t>(-1)) {
    massDataC_.closeFd();
    massDataC_.ContextClear();
    conn->shutdown();
  }

  ssize_t ret = read(massDataC_.getFd(), buf, massDataC_.getReadEveryTime());
  if (ret == 0) {
    conn->unsetHasMoreData();
    massDataC_.closeFd();
    massDataC_.ContextClear();

    //the file data is read completed.

  } else if (ret < 0) {
    if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
      return;
    }

    massDataC_.closeFd();
    massDataC_.ContextClear();
    conn->unsetHasMoreData();
    conn->shutdown();

    return;
  } else {
    conn->putDataToWriteBuffer(buf, ret);
    massDataC_.addSizeToRdstartAndUpdateRemind(ret);
    assert(massDataC_.getLength() == (massDataC_.getRdstart() + massDataC_.getRemaind()));

    if (massDataC_.getRemaind() == 0) {
      conn->unsetHasMoreData();
      massDataC_.closeFd();
      massDataC_.ContextClear();
      conn->shutdown();
      //the file data is read completed.

    }
  }

}

bool fas::http::HttpReqHandle::HandleError(TcpConnShreadPtr conn,
                                           const HttpRequest& req,
                                           const std::string& errorCode) {
  // FIXME : May be unreasonable.
  conn->shutdown();
  return false;
}

fas::http::HttpReqHandle::~HttpReqHandle() {
  //LOGGER_TRACE << "HttpReqHandle destroyed!" << fas::Log::CLRF;
}


fas::http::HttpReqHandle::SendMassDataContext::SendMassDataContext(int fd,
                                                                   int length,
                                                                   int rdstart) :
  fd_(fd),
  length_(length),
  rdstart_(rdstart),
  remaind_(length_ - rdstart_) {

}

void fas::http::HttpReqHandle::SendMassDataContext::ContextReset(int fd,
                                                                 int length,
                                                                 int rdstart) {
  fd_ = fd;
  length_ = length;
  rdstart_ = rdstart;
  remaind_ = length_ - rdstart_;
}

void fas::http::HttpReqHandle::SendMassDataContext::addSizeToRdstartAndUpdateRemind(ssize_t size) {
  rdstart_ += size;
  remaind_ -= size;
}

int fas::http::HttpReqHandle::SendMassDataContext::getFd() const {
  return fd_;
}

size_t fas::http::HttpReqHandle::SendMassDataContext::getLength() const {
  return length_;
}

size_t fas::http::HttpReqHandle::SendMassDataContext::getRdstart() const {
  return rdstart_;
}

size_t fas::http::HttpReqHandle::SendMassDataContext::getRemaind() const {
  return remaind_;
}

size_t fas::http::HttpReqHandle::SendMassDataContext::getReadEveryTime() const {
  return readEveryTime_;
}

void fas::http::HttpReqHandle::SendMassDataContext::ContextClear() {
  fd_ = 0;
  length_ = 0;
  rdstart_ = 0;
  remaind_ = 0;
}

void fas::http::HttpReqHandle::SendMassDataContext::closeFd() {
  ::close(fd_);
}
