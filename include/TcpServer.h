#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Handle.h"
#include "Socket.h"
#include "NetAddress.h"
#include "NetBaseTypes.h"

class TcpServer {
private:
  Socket serfd;
  Dispatcher *dispatch;
  Handle *handle;
public:
    TcpServer(NetAddress addr);
    TcpServer();
    ~TcpServer() = default;

    bool init(Dispatcher *dispatcher);

    Dispatcher* get_dispatcher();

    void handle_event(Events* event);
};

#endif // TCPSERVER_H
