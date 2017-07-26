#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <memory>
#include <map>
#include <vector>
#include <queue>


#include <Socket.h>
#include <Events.h>
#include <NetAddress.h>
#include <EventLoopThreadPool.h>
#include <ThreadPool.h>
#include <TcpConnection.h>
#include <Mutex.h>
#include <MutexLocker.h>
#include <SigIgnore.h>

namespace fas {

class EventLoop;
class Handle;
/*!
 * \brief The TcpServer class
 * The TcpServer class main job is manage TcpConnection and Handle.
 * TcpServer create TcpConnection in it's listenning socket's handread callback,
 * then established the relationship between TcpConnection and Handle,
 * The callbacks of TcpConnection's Handle was seted by TcpSerevr, then TcpServer
 * add TcpConnection's Handle to Io Thread's loop(workloop) and wake up this Io thread from polling.
 */
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
private:
    SigIgnore signor_;
    Socket server_;
    EventLoop *loop_;
    int threadNum_;
    Events events_;
    Handle *handle_;
    NetAddress addr_;
    const uint listenBacklog_;
    EventLoopThreadPool *loopThreadPool_;
    OnConnectionCallBack onConnectionCb_;
    TcpConnMessageCallback messageCb_;
};

}
#endif // FAS_TCPSERVER_H
