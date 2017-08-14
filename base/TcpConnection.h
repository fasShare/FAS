#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Buffer.h>


#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

namespace fas {

class Handle;
class EventLoop;
class buffer;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
public:
    typedef boost::shared_ptr<TcpConnection> TcpConnShreadPtr;
    typedef TcpConnection* TcpConnectionPtr;
    typedef boost::function<void (TcpConnShreadPtr, Buffer*, Timestamp)> TcpConnMessageCallback;
    typedef boost::function<void (TcpConnShreadPtr)> HasMoreDataCallback;
    typedef boost::function<void ()> CloseCallback;
    TcpConnection(EventLoop *loop,
            const Events& event,
            const NetAddress& peerAddr_,
            Timestamp now = Timestamp::now());
    ~TcpConnection();

    bool reinit(EventLoop* loop, const Events& event, const NetAddress& peerAddr, Timestamp now);
    bool reset();

    EventLoop* getLoop();
    
    int getConnfd() const;

    void setPeerAddr(const NetAddress& addr);
    
    void clearInLoop();

    void setOnMessageCallBack(const TcpConnMessageCallback& cb);
    bool messageCallbackVaild();
    void setOnCloseCallBack(const CloseCallback& cb);
    void SetHasMoreDataCallback(HasMoreDataCallback moreDataCb);

    void setHasMoreData();
    void unsetHasMoreData();
    
    void sendString(const std::string& msg);
    void sendData(const void *data, size_t len);
    void putDataToWriteBuffer(const void *data, size_t len);
    
    void handleRead(const Events& revents, Timestamp time);
    void handleWrite(const Events& revents, Timestamp time);
    void handleError(const Events& revents, Timestamp time);
    void handleClose(const Events& revents, Timestamp time);

    void shutdown();
    friend void shutdownFromThis(TcpConnShreadPtr conn);
private:
    EventLoop *loop_;
    Events event_;        /*!< events of connfd_ */
    boost::shared_ptr<Handle> handle_;
    Buffer *readBuffer_;
    Buffer *writeBuffer_;
    Socket connfd_;      /*!< connected socket. */
    NetAddress peerAddr_;
    bool shouldBeClosed_;
    bool closeing_;
    CloseCallback closeCb_;
    TcpConnMessageCallback messageCb_;
    Timestamp acceptTime_;
    bool sendAllDataOut_;

    //when you want to write mass data to TcpConnection,
    //to send those data in an optimal way you can use this flag and set a hasMoreDataCallback.
    bool hasMoreData_;
    HasMoreDataCallback moreDataCb_;
};
void shutdownFromThis(TcpConnection::TcpConnShreadPtr conn);
}
#endif // FAS_TCPCONNECTION_H

