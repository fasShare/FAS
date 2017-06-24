#ifndef FAS_MCACHETASKHANDLE_H
#define FAS_MCACHETASKHANDLE_H
#include <Timestamp.h>
#include <TcpConnection.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Handle.h>
#include <Buffer.h>
#include <TcpConnection.h>
#include <mcacheTask.h>

namespace fas {

namespace mdm {

class mcacheTaskHandle
{
public:
    using hashSPtr = ConsistencyHash::hashSPtr;
    using serNodeSptr = ConsistencyHash::serNodeSptr;
    using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
    mcacheTaskHandle(hashSPtr hash);
    ~mcacheTaskHandle();
    void OnMessageCallback(TcpConnShreadPtr conn, fas::Buffer* buffer, fas::Timestamp time);

    void sendDataToMemcached();
private:
    mcacheTask mTask_;
    hashSPtr hash_;
};

}

}

#endif // FAS_MCACHETASKHANDLE_H
