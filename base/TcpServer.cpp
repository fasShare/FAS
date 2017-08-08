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
#include <TcpConnBucket.h>
#include <EventLoopBucket.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpServer::TcpServer(fas::EventLoop* loop, const NetAddress& addr, int threadNum) :
    signor_(),
    server_(AF_INET, SOCK_STREAM, 0),
    loop_(loop),
    threadNum_(threadNum <= 0 ? 1: threadNum),
    events_(server_.getSocket(), kReadEvent),
    handle_(NULL),
    addr_(addr),
    listenBacklog_(50) {
    server_.setNoBlocking();
    server_.setExecClose();
    server_.bind(addr_);
    server_.listen(listenBacklog_);
	LOGGER_TRACE("server listen fd = " << server_.getSocket());
}

fas::EventLoop* fas::TcpServer::getLoop() const{
    return loop_;
}

void fas::TcpServer::setLoop(fas::EventLoop *loop) {
	loop_ = loop;
}

bool fas::TcpServer::start() {  
	if (!loop_) {
		LOGGER_ERROR("TcpServer's loop is nullptr.");
		return false;
	}
    handle_ = new (std::nothrow) Handle(loop_, events_);
    if (!handle_) {
        LOGGER_ERROR("new server handle_ error!");
        return false;
    }

    handle_->setHandleRead(boost::bind(&TcpServer::defHandleAccept, this, _1, _2));
    loop_->addHandle(handle_);

    threadPool_ = new (std::nothrow) ThreadPool(threadNum_, TcpServer::LoopThreadFunc, "ThreadPool");
    if (!threadPool_) {
        return false;
    }
    return  threadPool_->start();
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
        workloop = EventLoopBucket::Instance()->getNextLoop();
        if (nullptr == workloop) {
            LOGGER_SYSERR("In Tcpserver accepted return an error!");
            return;
        }

        fas::TcpServer::TcpConnShreadPtr sconn(new fas::TcpConnection(workloop,
                    fas::Events(sd, kReadEvent),
                    peerAddr,
                    acceptTime));
        fas::TcpConnBucket *bucket = TcpConnBucket::Instance();
        connkey_t key = fas::TcpServer::connkey_t(sd, sconn.get());
        long looptid = workloop->getTid();
        LOGGER_TRACE("loop tid " << looptid);
        bucket->addTcpConnection(looptid, key, sconn);
        sconn->setOnCloseCallBack(boost::bind(&TcpConnBucket::removeTcpConnection, bucket, looptid, key));
    
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

void fas::TcpServer::setMessageCallback(const TcpConnMessageCallback& cb) {
    messageCb_  = cb;
}

void fas::TcpServer::LoopThreadFunc() {
    EventLoop *loop = new (std::nothrow) EventLoop();
    if (nullptr == loop) {
        return;
    }

    if (!EventLoopBucket::Instance()->addEventLoop(gettid(), loop)) {
        return;
    }

    loop->loop();
}

fas::TcpServer::~TcpServer() {
    delete handle_;
    handle_ = nullptr;
    LOGGER_TRACE("TcpServer will be destroyed in process " << getpid());
}
