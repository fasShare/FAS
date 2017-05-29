#include <iostream>
#include <memory>
#include <unistd.h>


#include <Socket.h>
#include <Timestamp.h>
#include <Handle.h>
#include <Log.h>
#include <EventLoop.h>
#include <TcpServer.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpServer::TcpServer(fas::EventLoop* loop,
                          const NetAddress& addr,
                          uint threadNum) :
  server_(AF_INET, SOCK_STREAM, 0),
  loop_(loop),
  events_(server_.getSocket(), kReadEvent),
  handle_(NULL),
  addr_(addr),
  listenBacklog_(50),
  threadPool_(loop_, threadNum),
  conns_() {
  assert(loop_ != NULL);
  server_.setNoBlocking();
  server_.setExecClose();
  handle_ = new Handle(loop_, events_);
  assert(handle_->getLoop() == loop_);
  server_.bind(addr_);
  server_.listen(listenBacklog_);
}

fas::EventLoop* fas::TcpServer::getLoop() const{
  assert(loop_ != NULL);
  return loop_;
}

fas::TcpConnShreadPtr fas::TcpServer::getConn(fas::connkey_t key) const {
  return conns_.find(key)->second;
}

fas::TcpConnShreadPtr fas::TcpServer::getConn(fas::connkey_t key) {
  return conns_.find(key)->second;
}

bool fas::TcpServer::start() {
  handle_->setHandleRead(boost::bind(&TcpServer::handleReadEvent, this, _1, _2));
  loop_->addHandle(handle_);
  threadPool_.start();
  // FIXME : use Log
  LOGGER_TRACE << "Current EventLoop num is " << loop_->getCount() << Log::CLRF;
  return true;
}

void fas::TcpServer::handleReadEvent(const fas::Events& event, fas::Timestamp time) {
  LOGGER_TRACE << " start !" << Log::CLRF;
  loop_->assertInOwnerThread();
  boost::ignore_unused(time);
  fas::EventLoop *workloop = loop_;
  if (this->threadPool_.getThreadNum() > 0) {
    workloop = threadPool_.getNextEventLoop();
  }

  assert(event.getFd() == server_.getSocket());

  fas::NetAddress peerAddr;
  Timestamp acceptTime = Timestamp::now();
  Socket_t sd = server_.accept(peerAddr, true);
  // FIXME : if sd < 0
  if (sd < 0) {
    // FIXME : no safe.
    LOGGER_SYSERR << "In Tcpserver accepted return an error!" << Log::CLRF;
    return;
  }
  //conn will be destroy if there are not other shared_ptr
  //increase the refcount of it
//  fas::TcpConnectionPtr conn = new fas::TcpConnection(workloop,
//                                                    fas::Events(sd, kReadEvent),
//                                                    peerAddr,
//                                                    acceptTime);

  fas::TcpConnShreadPtr sconn(new fas::TcpConnection(workloop,
                                                     fas::Events(sd, kReadEvent),
                                                     peerAddr,
                                                     acceptTime));
  conns_[fas::connkey_t(sd, sconn.get())] = sconn;
  sconn->setOnCloseCallBack(boost::bind(&TcpServer::removeConnection, this, fas::connkey_t(sd, sconn.get())));
  if (this->onConnectionCb_) {
  // conn's messagecallback should be seted.
    this->onConnectionCb_(sconn);
  }

  if (!sconn->messageCallbackVaild()) {
    sconn->setOnMessageCallBack(messageCb_);
  }

  workloop->wakeUp();
  LOGGER_TRACE << " end !" << Log::CLRF;
}

void fas::TcpServer::setOnConnectionCallBack(OnConnectionCallBack onConnectionCb) {
  this->onConnectionCb_ = onConnectionCb;
}

void fas::TcpServer::setOnConnRemovedCallBack(OnConnectionRemovedCallBack onConnRemovedCb) {
  LOGGER_TRACE << Log::CLRF;
  this->onConnRemovedCb_ = onConnRemovedCb;
}

void fas::TcpServer::setMessageCallback(const fas::MessageCallback& cb) {
  messageCb_  = cb;
}

void fas::TcpServer::removeConnection(fas::connkey_t key) {
  LOGGER_TRACE << Log::CLRF;
  loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, key));
  LOGGER_TRACE << " out " <<Log::CLRF;
}

void fas::TcpServer::removeConnectionInLoop(fas::connkey_t key) {
  LOGGER_TRACE << Log::CLRF;
  loop_->assertInOwnerThread();
  //must be have this statement.
  if (this->onConnRemovedCb_) {
    this->onConnRemovedCb_(key);
  }
  auto iter = conns_.find(key);
  if (iter == conns_.end()) {
    return;
  }
  fas::TcpConnShreadPtr conn = conns_.find(key)->second;
  size_t n = conns_.erase(key);
  boost::ignore_unused(n);
  assert(n == 1);
  fas::EventLoop* ioLoop = conn->getLoop();
  LOGGER_TRACE << "ioLoop->queueInLoop" << Log::CLRF;
  ioLoop->queueInLoop(
      boost::bind(&TcpConnection::closeAndClearTcpConnection, conn));
  LOGGER_TRACE << "after ioLoop->queueInLoop" << Log::CLRF;
}


fas::TcpServer::~TcpServer() {
  delete handle_;
}
