#ifndef FAS_MCACHETASKSERVER_H
#define FAS_MCACHETASKSERVER_H
#include <TcpServer.h>
#include <McacheTaskHandle.h>
#include <EventLoop.h>
#include <NetAddress.h>
#include <TcpServer.h>

namespace fas {

namespace mdm {

class McacheTaskServer
{
public:
    using HashSPtr = ConsistencyHash::HashSPtr;
    using SerNodeSptr = ConsistencyHash::SerNodeSptr;
    using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
    using connkey_t = fas::TcpServer::connkey_t;
    McacheTaskServer(HashSPtr hash, fas::EventLoop* loop, \
            const fas::NetAddress& addr, int threadNum);
    ~McacheTaskServer();

    void OnNewConnection(TcpConnShreadPtr conn);

    bool start();
private:
    fas::EventLoop *loop_;
    fas::TcpServer tcpSer_;
    HashSPtr hash_;
};

}

}

#endif // FAS_MCACHETASKSERVER_H
