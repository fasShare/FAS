#ifndef FAS_HTTPREQHANDLE_H
#define FAS_HTTPREQHANDLE_H
#include <string>


#include <HttpResponse.h>
#include <HttpRequest.h>
#include <HttpCommon.h>

namespace fas {

namespace http {

class HttpReqHandle {
public:
  HttpReqHandle();
  ~HttpReqHandle();

  void OnMessageCallback(TcpConnection *conn, Buffer* buffer, Timestamp time);
  bool handleMethod(TcpConnection *conn, const HttpRequest& req);

  bool HandleGet(TcpConnection *conn, const HttpRequest& req);
  bool HandlePut(TcpConnection *conn, const HttpRequest& req);
  bool HandlePost(TcpConnection *conn, const HttpRequest& req);
  bool HandleTrace(TcpConnection *conn, const HttpRequest& req);
  bool HandleHead(TcpConnection *conn, const HttpRequest& req);
  bool HandleDelete(TcpConnection *conn, const HttpRequest& req);
  bool HandleOptions(TcpConnection *conn, const HttpRequest& req);

  void sendMassData(TcpConnection *conn);

  bool HandleError(TcpConnection *conn, const HttpRequest& req, const std::string& errorCode);

  struct SendMassDataContext {
    int fd_;
    size_t length_;
    size_t rdstart_;  //The value is the offset of next read.
    size_t remaind_;
    const static size_t readEveryTime_;
  };

private:
  HttpRequest request_;
  HttpResponse response_;
  ServerOptions options_;

  SendMassDataContext massDataC_;

  bool handingMethod_;
};

}

}

#endif // HTTPREQHANDLE_H

