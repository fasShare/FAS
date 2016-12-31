#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Executor.h"
#include "NetAddress.h"
#include "NetBaseTypes.h"

class TcpServer : public Executor
{
public:
    TcpServer(NetAddress addr);
    TcpServer();
    ~TcpServer() = default;
    virtual void Executor_handle_event(Events* event) override;
};

#endif // TCPSERVER_H
