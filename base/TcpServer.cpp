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
                     const fas::NetAddress& addr) :
  server_(AF_INET, SOCK_STREAM, 0),
  loop_(loop),
  events_(server_.getSocket(), kReadEvent),
  handle_(NULL),
  addr_(addr),
  listenBacklog_(50),
  threadPool_(loop_, 4),
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

fas::TcpConnShreadPtr fas::TcpServer::getConn(fas::map_conn_key_t key) const {
  return conns_.find(key)->second;
}
fas::TcpConnShreadPtr fas::TcpServer::getConn(fas::map_conn_key_t key) {
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

void fas::TcpServer::handleReadEvent(fas::Events event, fas::Timestamp time) {
  loop_->assertInOwnerThread();
  boost::ignore_unused(time);
  fas::EventLoop *workloop = threadPool_.getNextEventLoop();
  assert(event.getFd() == server_.getSocket());

  fas::NetAddress peerAddr;
  Socket_t sd = server_.accept(peerAddr, true);
  // FIXME : if sd < 0
  if (sd < 0) {
    // FIXME : no safe.
    LOGGER_SYSERR << "In Tcpserver accepted return an error!" << Log::CLRF;
    return;
  }
  //conn will be destroy if there are not other shared_ptr
  //increase the refcount of it
  fas::TcpConnShreadPtr conn = fas::getSharedPtr(new fas::TcpConnection(workloop, fas::Events(sd, kReadEvent)));

  conn->setOnMessageCallBack(messageCb_);
  conn->setOnCloseCallBack(boost::bind(&TcpServer::removeConnection, this, sd));

  conns_[sd] = conn;
  workloop->wakeUp();

  //FIXME : use Log output debug msg.
}

void fas::TcpServer::setMessageCallback(const fas::MessageCallback& cb) {
  messageCb_  = cb;
}

void fas::TcpServer::removeConnection(fas::map_conn_key_t key) {
  loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, key));
}

void fas::TcpServer::removeConnectionInLoop(fas::map_conn_key_t key) {
  loop_->assertInOwnerThread();
  //must be have this statement.
  fas::TcpConnShreadPtr conn = conns_.find(key)->second;
  size_t n = conns_.erase(key);
  boost::ignore_unused(n);
  assert(n == 1);
  fas::EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      boost::bind(&TcpConnection::closeAndClearTcpConnection, conn));
}


fas::TcpServer::~TcpServer() {
  delete handle_;
}
