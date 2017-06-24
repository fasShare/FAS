#ifndef FAS_MCACHETASKSERVER_H
#define FAS_MCACHETASKSERVER_H
#include <TcpServer.h>
#include <mcacheTaskHandle.h>
#include <EventLoop.h>
#include <NetAddress.h>
#include <TcpServer.h>

namespace fas {

namespace mdm {

class mcacheTaskServer
{
public:
    using hashSPtr = ConsistencyHash::hashSPtr;
    using serNodeSptr = ConsistencyHash::serNodeSptr;
    using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
    using connkey_t = fas::TcpServer::connkey_t;
    mcacheTaskServer(hashSPtr hash, fas::EventLoop* loop, \
            const fas::NetAddress& addr, int threadNum);
    ~mcacheTaskServer();

    void OnNewConnection(TcpConnShreadPtr conn);
    void OnConnectionRemoved(connkey_t conn);

    bool start();
private:
    fas::EventLoop *loop_;
    fas::TcpServer tcpSer_;
    std::map<connkey_t, std::shared_ptr<mcacheTaskHandle>> reqHandles_;
    hashSPtr hash_;
};

}

}

#endif // FAS_MCACHETASKSERVER_H
