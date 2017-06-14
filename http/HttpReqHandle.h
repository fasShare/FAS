#ifndef FAS_HTTPREQHANDLE_H
#define FAS_HTTPREQHANDLE_H
#include <string>


#include <TcpConnection.h>
#include <HttpResponse.h>
#include <HttpRequest.h>
#include <HttpCommon.h>

namespace fas {

namespace http {

class HttpReqHandle {
public:
using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
  HttpReqHandle();
  ~HttpReqHandle();

  void OnMessageCallback(TcpConnShreadPtr conn, Buffer* buffer, Timestamp time);
  bool handleMethod(TcpConnShreadPtr conn, const HttpRequest& req);

  bool HandleGet(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandlePut(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandlePost(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandleTrace(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandleHead(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandleDelete(TcpConnShreadPtr conn, const HttpRequest& req);
  bool HandleOptions(TcpConnShreadPtr conn, const HttpRequest& req);

  void sendMassData(TcpConnShreadPtr conn);

  bool HandleError(TcpConnShreadPtr conn, const HttpRequest& req, const std::string& errorCode);

  class SendMassDataContext {
  public:
    SendMassDataContext() = default;
    SendMassDataContext(int fd, int length, int rdstart);

    void ContextReset(int fd, int length, int rdstart);

    void addSizeToRdstartAndUpdateRemind(ssize_t size);

    int getFd() const;
    size_t getLength() const;
    size_t getRdstart() const;
    size_t getRemaind() const;
    size_t getReadEveryTime() const;

    void ContextClear();
    void closeFd();
  private:
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

