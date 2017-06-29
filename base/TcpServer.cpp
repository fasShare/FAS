#include <iostream>
#include <memory>
#include <unistd.h>
#include <new>


#include <Socket.h>
#include <Timestamp.h>
#include <Handle.h>
#include <Log.h>
#include <EventLoop.h>
#include <TcpServer.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpServer::TcpServer(fas::EventLoop* loop, const NetAddress& addr, int threadNum) :
    server_(AF_INET, SOCK_STREAM, 0),
    loop_(loop),
    threadNum_(threadNum <= 0 ? 1: threadNum),
    events_(server_.getSocket(), kReadEvent),
    handle_(NULL),
    addr_(addr),
    listenBacklog_(50),
    conns_(),
    handleQueue_(nullptr),
    handleMap_(nullptr) {
    assert(loop_ != NULL);
    server_.setNoBlocking();
    server_.setExecClose();
    server_.bind(addr_);
    server_.listen(listenBacklog_);
}

fas::EventLoop* fas::TcpServer::getLoop() const{
    assert(loop_ != NULL);
    return loop_;
}

fas::TcpServer::TcpConnShreadPtr fas::TcpServer::getConn(fas::TcpServer::connkey_t key) const {
    return conns_.find(key)->second;
}

fas::TcpServer::TcpConnShreadPtr fas::TcpServer::getConn(fas::TcpServer::connkey_t key) {
    return conns_.find(key)->second;
}

bool fas::TcpServer::start() {  
    handle_ = new (std::nothrow) Handle(loop_, events_);
    if (!handle_) {
        LOGGER_ERROR("new server handle_ error!");
        return false;
    }

    handle_->setHandleRead(boost::bind(&TcpServer::defHandleAccept, this, _1, _2));
    loop_->addHandle(handle_);

    loopThreadPool_ = new (std::nothrow) EventLoopThreadPool(loop_, threadNum_,\
            "EventLoopThreadPool");
    if (!loopThreadPool_) {
        return false;
    }
    return  loopThreadPool_->start();
}

void fas::TcpServer::defHandleAccept(const fas::Events& event, fas::Timestamp time) {
    loop_->assertInOwnerThread();
    boost::ignore_unused(time);

    fas::EventLoop *workloop = loop_;

    if (event.getFd() == server_.getSocket()) {
        fas::NetAddress peerAddr;
        Timestamp acceptTime = Timestamp::now();
        int sd = server_.accept(peerAddr, true);
        if (sd < 0) {
            LOGGER_SYSERR("In Tcpserver accepted return an error!");
            return;
        }
        workloop = loopThreadPool_->getNextEventLoop();

        fas::TcpServer::TcpConnShreadPtr sconn(new fas::TcpConnection(workloop,
                    fas::Events(sd, kReadEvent),
                    peerAddr,
                    acceptTime));
        conns_[fas::TcpServer::connkey_t(sd, sconn.get())] = sconn;
        sconn->setOnCloseCallBack(boost::bind(&TcpServer::removeConnection, this,
                    fas::TcpServer::connkey_t(sd, sconn.get())));
        if (this->onConnectionCb_) {
            this->onConnectionCb_(sconn);
        }

        if (!sconn->messageCallbackVaild()) {
            sconn->setOnMessageCallBack(messageCb_);
        }

        workloop->wakeUp();
    } else {
        LOGGER_ERROR("event.getFd() == server_.getSocket()");
    }
}

void fas::TcpServer::setOnConnectionCallBack(OnConnectionCallBack onConnectionCb) {
    this->onConnectionCb_ = onConnectionCb;
}

void fas::TcpServer::setOnConnRemovedCallBack(OnConnectionRemovedCallBack onConnRemovedCb) {
    LOGGER_TRACE("TcpServer::setOnConnRemovedCallBack");
    this->onConnRemovedCb_ = onConnRemovedCb;
}

void fas::TcpServer::setMessageCallback(const TcpConnMessageCallback& cb) {
    messageCb_  = cb;
}

void fas::TcpServer::removeConnection(fas::TcpServer::connkey_t key) {
    loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, key));
}

void fas::TcpServer::removeConnectionInLoop(fas::TcpServer::connkey_t key) {
    loop_->assertInOwnerThread();
    if (this->onConnRemovedCb_) {
        this->onConnRemovedCb_(key);
    } else {
        LOGGER_DEBUG("On connecction remove callback is invaild!");
    }
    auto iter = conns_.find(key);
    if (iter == conns_.end()) {
        return;
    }
    fas::TcpServer::TcpConnShreadPtr conn = conns_.find(key)->second;
    if (!conn) {
        LOGGER_ERROR("can't find conn in conns!");
    }
    size_t n = conns_.erase(key);
    if (n != 1) {
        LOGGER_ERROR("connectin erase ret != 1.");
    }
    fas::EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(boost::bind(&TcpConnection::closeAndClearTcpConnection, conn));
}


fas::TcpServer::~TcpServer() {
    delete handle_;
    handle_ = nullptr;
}
