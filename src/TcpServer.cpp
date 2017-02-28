#include <iostream>
#include <memory>
#include <unistd.h>


#include <TcpServer.h>
#include <Socket.h>
#include <Timestamp.h>
#include <Handle.h>
#include <Log.h>
#include <EventLoop.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

TcpServer::TcpServer(EventLoop* loop,
                     const NetAddress& addr) :
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

EventLoop* TcpServer::getLoop() const{
  assert(loop_ != NULL);
  return loop_;
}

TcpConnShreadPtr TcpServer::getConn(map_conn_key_t key) const {
  return conns_.find(key)->second;
}
TcpConnShreadPtr TcpServer::getConn(map_conn_key_t key) {
  return conns_.find(key)->second;
}

bool TcpServer::start() {
  handle_->setHandleRead(boost::bind(&TcpServer::handleReadEvent, this, _1, _2));
  loop_->addHandle(handle_);
  threadPool_.start();
  // FIXME : use Log
  std::cout << "Current EventLoop num is " << loop_->getCount() << std::endl;
  return true;
}

void TcpServer::handleReadEvent(Events event, Timestamp time) {
  loop_->assertInOwnerThread();
  boost::ignore_unused(time);
  EventLoop *workloop = threadPool_.getNextEventLoop();
  assert(event.getFd() == server_.getSocket());

  NetAddress peerAddr;
  Socket_t sd = server_.accept(peerAddr, true);
  // FIXME : if sd < 0
  if (sd < 0) {
    // FIXME : no safe.
    LOG_SYSERR("In Tcpserver accepted return an error!");
    return;
  }
  //conn will be destroy if there are not other shared_ptr
  //increase the refcount of it
  TcpConnShreadPtr conn = getSharedPtr(new TcpConnection(workloop, Events(sd, kReadEvent)));

  conn->setOnMessageCallBack(messageCb_);
  conn->setOnCloseCallBack(boost::bind(&TcpServer::removeConnection, this, sd));

  conns_[sd] = conn;
  workloop->wakeUp();

  //FIXME : use Log output debug msg.
}

void TcpServer::setMessageCallback(const MessageCallback& cb) {
  messageCb_  = cb;
}

void TcpServer::removeConnection(map_conn_key_t key) {
  loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, key));
}

void TcpServer::removeConnectionInLoop(map_conn_key_t key) {
  loop_->assertInOwnerThread();
  //must be have this statement.
  TcpConnShreadPtr conn = conns_.find(key)->second;
  size_t n = conns_.erase(key);
  boost::ignore_unused(n);
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      boost::bind(&TcpConnection::closeAndClearTcpConnection, conn));
}


TcpServer::~TcpServer() {
  delete handle_;
}
