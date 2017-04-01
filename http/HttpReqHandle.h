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

  bool sendBigFile(TcpConnection *conn, const std::string& filename);
  bool sendLittleFile(TcpConnection *conn, const std::string& filename);

  bool HandleError(TcpConnection *conn, const HttpRequest& req, const std::string& errorCode);
private:
  HttpRequest request_;
  HttpResponse response_;
  ServerOptions options_;
};

}

}

#endif // HTTPREQHANDLE_H

