#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Executor.h"
#include "NetAddress.h"
#include "NetBaseTypes.h"

class TcpServer : public Executor
{
private:
  Socket serfd;
public:
    TcpServer(NetAddress addr);
    TcpServer();
    ~TcpServer() = default;
    void Executor_handle_event(Events* event);
};

#endif // TCPSERVER_H
