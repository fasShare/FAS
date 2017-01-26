#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include "Handle.h"
#include "Socket.h"
#include "NetAddress.h"
#include "Types.h"

class TcpServer {
public:
    TcpServer(NetAddress addr);
    TcpServer();
    ~TcpServer() = default;

    bool init(Dispatcher *dispatcher);

    Dispatcher* get_dispatcher();

    void handle_read_event(Events* event, Timestamp time);

private:
  Socket_t server_socket_;
  Dispatcher *dispatch_;
  Handle *handle_;
};

#endif // FAS_TCPSERVER_H
