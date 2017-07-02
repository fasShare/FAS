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
    typedef std::pair<int, TcpConnectionPtr> connkey_t;

    typedef boost::function<void (TcpConnShreadPtr)> OnConnectionCallBack;
    typedef boost::function<void (connkey_t)> OnConnectionRemovedCallBack;

    TcpServer(EventLoop *loop, const NetAddress& addr, int threadNum = 4);
    ~TcpServer();

    EventLoop* getLoop() const;

	void resetLoop(fas::EventLoop *loop);

    TcpConnShreadPtr getConn(connkey_t key) const;
    TcpConnShreadPtr getConn(connkey_t key);

    void EnterQueue(boost::shared_ptr<Handle>& handle);

    bool pushHandleToQueue(boost::shared_ptr<Handle>& handle);
    bool getHandleFromQueue(boost::shared_ptr<Handle>& handle);
    bool removeHandleFromQueue(boost::shared_ptr<Handle>& handle);

    bool start();
    /*!
     * \brief handleReadEvent
     * \param event
     * \param time
     * This function is accept socket's callback.
     * when a new client connect to this TcpServer, it'll be called.
     * We create new TcpConnection in it.
     */
    void defHandleAccept(const Events& event, Timestamp time);
    void setOnConnectionCallBack(OnConnectionCallBack onConnectionCb);
    void setOnConnRemovedCallBack(OnConnectionRemovedCallBack onConnRemovedCb);
    /*!
     * \brief setMessageCallback
     * \param cb
     * This function set the message's callback of TcpConnection was created in
     * function TcpServer::handleReadEvent.
     * The MessageCallback will be called when TcpConnection's connected socket can be read, there are
     * a message come.
     */
    void setMessageCallback(const TcpConnMessageCallback& cb);
    /*!
     * \brief removeConnection
     * \param conn
     * \see removeConnectionInLoop
     * When TcpConnection's will be closed this function will be called.
     */
    void removeConnection(connkey_t conn);
    void removeConnectionInLoop(connkey_t conn);
private:
    Socket server_;
    EventLoop *loop_;
    int threadNum_;
    Events events_;
    Handle *handle_;
    NetAddress addr_;
    const uint listenBacklog_;
    EventLoopThreadPool *loopThreadPool_;
    std::map<connkey_t, boost::shared_ptr<TcpConnection> > conns_;
    OnConnectionCallBack onConnectionCb_;
    OnConnectionRemovedCallBack onConnRemovedCb_;
    TcpConnMessageCallback messageCb_;

    ThreadPool *threadPool_;
    std::queue<boost::shared_ptr<Handle>> *handleQueue_;
    std::map<int, boost::shared_ptr<Handle>> *handleMap_;
};

}
#endif // FAS_TCPSERVER_H
