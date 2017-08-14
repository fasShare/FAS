#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <memory>
#include <map>
#include <vector>
#include <queue>


#include <Socket.h>
#include <Events.h>
#include <NetAddress.h>
#include <ThreadPool.h>
#include <TcpConnection.h>
#include <Mutex.h>
#include <MutexLocker.h>
#include <SigIgnore.h>

namespace fas {

class EventLoop;
class Handle;

class TcpServer {
public:
    using TcpConnShreadPtr = TcpConnection::TcpConnShreadPtr;
    using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;
    using TcpConnMessageCallback = TcpConnection::TcpConnMessageCallback;
    using connkey_t = std::pair<int, TcpConnectionPtr>;

    using OnConnectionCallBack = boost::function<void (TcpConnShreadPtr)>;
    using OnConnectionRemovedCallBack = boost::function<void (connkey_t)>;

    TcpServer(EventLoop *loop, const NetAddress& addr, int threadNum = 4);
    ~TcpServer();

    EventLoop* getLoop() const;

	void setLoop(fas::EventLoop *loop);

    bool start();
    void defHandleAccept(const Events& event, Timestamp time);
    void setOnConnectionCallBack(OnConnectionCallBack onConnectionCb);
    void setMessageCallback(const TcpConnMessageCallback& cb);

    static void LoopThreadFunc();
private:
    SigIgnore signor_;
    Socket server_;
    EventLoop *loop_;
    int threadNum_;
    Events events_;
    boost::shared_ptr<Handle> handle_;
    NetAddress addr_;
    const uint listenBacklog_;
    ThreadPool *threadPool_;
    OnConnectionCallBack onConnectionCb_;
    TcpConnMessageCallback messageCb_;
};

}
#endif // FAS_TCPSERVER_H
