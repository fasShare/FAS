#ifndef FAS_MCACHETASKHANDLE_H
#define FAS_MCACHETASKHANDLE_H
#include <Timestamp.h>
#include <TcpConnection.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Handle.h>
#include <Buffer.h>
#include <TcpConnection.h>
#include <McacheTask.h>

namespace fas {

namespace mdm {

class McacheTaskHandle
{
public:
    using HashSPtr = ConsistencyHash::HashSPtr;
    using SerNodeSptr = ConsistencyHash::SerNodeSptr;
    using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
    McacheTaskHandle(HashSPtr hash);
    ~McacheTaskHandle();
    void OnMessageCallback(TcpConnShreadPtr conn, boost::shared_ptr<fas::Buffer> buffer, fas::Timestamp time);

    void sendDataToMemcached();
private:
    McacheTask mTask_;
    HashSPtr hash_;
};

}

}

#endif // FAS_MCACHETASKHANDLE_H
