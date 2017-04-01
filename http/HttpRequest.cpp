#include <HttpRequest.h>
#include <utilstring.h>
#include <Log.h>


#include <boost/core/ignore_unused.hpp>

fas::http::HttpRequest::HttpRequest() :
  requestState_(ReqState::BAD) {
}

bool fas::http::HttpRequest::analyseHttpRequestHeader(Buffer *buffer) {
  const char* pos = buffer->findChars("\r\n\r\n", 4);
  if (pos == NULL) {
    // The HttpRequest header is not completed.
    this->setReqestState(HttpRequest::ReqState::INCOMPLETE);
    return false;
  }

  string line;
  bool firstLine = true;

  while (true) {
    line = buffer->retrieveAsString(buffer->findCRLF() - buffer->peek());

    if (firstLine) {
      firstLine = false;
      std::vector<string> ret;
      fas::utils::StringSplitBychar(line, ' ', ret, 0);
      if (ret.size() != 3) {
        this->setReqestState(HttpRequest::ReqState::BAD);
        return false;
      }
      this->method_ = fas::utils::StringTrim(ret.at(0));
      this->path_ = fas::utils::StringTrim(ret.at(1));
      this->version_ = fas::utils::StringTrim(ret.at(2));
      this->path_ = fas::utils::StringRemoveRepeatChar(this->path_, "/");
    } else {
      size_t splitPos = line.find(':');
      headers_.insert({fas::utils::StringTrim(line.substr(0, splitPos)),
                       fas::utils::StringTrim(line.substr(splitPos + 1,
                                                          line.length() - splitPos - 1))});
    }

    if (buffer->peek() == pos) {
      buffer->retrieve(4);  //remove "\r\n\r\n"
      break;
    }
    buffer->retrieve(2); //remove "\r\n"
  }
  this->setReqestState(HttpRequest::ReqState::GOOD);
  return true;
}

bool fas::http::HttpRequest::analyseHttpRequestBody(Buffer *buffer) {
  boost::ignore_unused(buffer);
  return true;
}

void fas::http::HttpRequest::setReqestState(ReqState state) {
  this->requestState_ = state;
}

fas::http::HttpRequest::ReqState fas::http::HttpRequest::getReqestState() const {
  return this->requestState_;
}

std::string fas::http::HttpRequest::getMethod() const {
  return this->method_;
}

std::string fas::http::HttpRequest::getPath() const {
  return this->path_;
}

std::string fas::http::HttpRequest::getVersion() const {
  return this->version_;
}

fas::http::HttpRequest::~HttpRequest() {
  LOGGER_TRACE << "fas::http::~HttpRequest()" << fas::Log::CLRF;
}
