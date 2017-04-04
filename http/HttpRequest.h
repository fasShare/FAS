#ifndef FAS_HTTPREQUEST_H
#define FAS_HTTPREQUEST_H
#include <map>


#include <Types.h>
#include <Timestamp.h>

namespace fas {

class Buffer;
class Timestamp;
class TcpConnection;

namespace http {

class HttpRequest {
public:

  enum ReqState {
    BAD,
    GOOD,
    INCOMPLETE,
  };

  HttpRequest();
  ~HttpRequest();

  bool analyseHttpRequestHeader(Buffer *buffer);

  bool analyseHttpRequestBody(Buffer *buffer);

  void setReqestState(ReqState state);
  ReqState getReqestState() const;

  const std::map<std::string, std::string>& getHeaders() const;

  std::string getMethod() const;
  std::string getPath() const;
  std::string getVersion() const;
private:
  ReqState requestState_;

  std::string method_;
  std::string path_;
  std::string version_;

  std::map<std::string, std::string> headers_;
};


}

}

#endif // FAS_HTTPREQUEST_H

