#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Handle.h"
#include "Socket.h"
#include "NetAddress.h"
#include "NetBaseTypes.h"

class TcpServer {
private:
  Socket_t server_socket_;
  Dispatcher *dispatch_;
  Handle *handle_;
public:
    TcpServer(NetAddress addr);
    TcpServer();
    ~TcpServer() = default;

    bool init(Dispatcher *dispatcher);

    Dispatcher* get_dispatcher();

    void handle_event(Events* event, Timestamp time);
};

#endif // TCPSERVER_H
