#ifndef FAS_TCPCONNBUCKET_H
#define FAS_TCPCONNBUCKET_H
#include <map>
#include <sys/types.h>

#include <TcpConnection.h>
#include <Mutex.h>

#include <boost/shared_ptr.hpp>

namespace fas {
    
class TcpConnBucket {
using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;
using TcpConnShreadPtr = TcpConnection::TcpConnShreadPtr;
typedef std::pair<int, TcpConnectionPtr> connkey_t;
public:
    TcpConnBucket() = default;
    bool addTcpConnection(long tid, connkey_t key, boost::shared_ptr<TcpConnection>& conn);
    void removeTcpConnection(long tid, connkey_t conn);
private:
    Mutex mutex_;
    std::map<long, std::map<connkey_t, boost::shared_ptr<TcpConnection>>> conns_; 
};

}

#endif //FAS_TCPCONNBUCKET_H
