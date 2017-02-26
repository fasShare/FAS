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
  server_(SocketNoBlockingOrExec(AF_INET, SOCK_STREAM, 0)),
  loop_(loop),
  events_(server_, kReadEvent),
  handle_(NULL),
  addr_(addr),
  listenBacklog_(50),
  threadPool_(loop_, 4),
  conns_() {

  assert(loop_ != NULL);
  handle_ = new Handle(loop_, events_);
  assert(handle_->getLoop() == loop_);
  SocketBind(server_, addr_);
  SocketListen(server_, listenBacklog_);
}

EventLoop* TcpServer::getLoop() const{
  assert(loop_ != NULL);
  return loop_;
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
  LOG_TRACE("TcpServer::handleReadEvent");
  loop_->assertInOwnerThread();
  EventLoop *workloop = threadPool_.getNextEventLoop();

  int sd = ::accept(event.getFd(), NULL, NULL);
  // FIXME : if sd < 0
  assert(SetNoBlockingOrExec(sd)); // FIXME : if error

  //conn will be destroy if there are not other shared_ptr
  //increase the refcount of it
  TcpConnShreadPtr conn = getSharedPtr(new TcpConnection(workloop, Events(sd, kReadEvent)));
  conn->setOnMessageCallBack(messageCb_);
  conn->setOnCloseCallBack(boost::bind(&TcpServer::removeConnection, this, conn));

  conns_[sd] = conn;
  workloop->wakeUp();

  //FIXME : use Log output debug msg.
}

void TcpServer::setMessageCallback(const MessageCallback& cb) {
  messageCb_  = cb;
}

void TcpServer::removeConnection(TcpConnShreadPtr conn) {
  loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(TcpConnShreadPtr conn) {
  loop_->assertInOwnerThread();
  size_t n = conns_.erase(conn->getConnfd());
  boost::ignore_unused(n);
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      boost::bind(&TcpConnection::closeAndClearTcpConnection, conn));
}


TcpServer::~TcpServer() {
  delete handle_;
}
