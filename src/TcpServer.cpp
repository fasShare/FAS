#include <TcpServer.h>
#include <iostream>
#include <Socket.h>
#include <unistd.h>
#include <boost/bind.hpp>
#include <Timestamp.h>

using namespace std;

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
  cout << "Current EventLoop num is " << loop_->getCount() << endl;
  return true;
}

void TcpServer::handleReadEvent(Events* event, Timestamp time) {
  loop_->assertInOwner();
  EventLoop *workloop = threadPool_.getNextEventLoop();

  int sd = ::accept(event->getFd(), NULL, NULL);
  assert(sd > 0);
  // FIXME : if sd < 0
  assert(SetNoBlockingOrExec(sd)); // FIXME : if error

  TcpConnection *conn = new TcpConnection(workloop, Events(sd, kReadEvent));

  conn->setHandleRead(boost::bind(&TcpConnection::defaultHandleRead, conn, _1, _2));
  conn->setHandleWrite(boost::bind(&TcpConnection::defaultHandleWrite, conn, _1, _2));
  conn->setHandleError(boost::bind(&TcpConnection::defaultHandleError, conn, _1, _2));
  conn->setHandleClose(boost::bind(&TcpConnection::defaultHandleClose, conn, _1, _2));
  conns_[sd] = conn;

  cout << "Current connection num is " << conns_.size() << endl;
}

TcpServer::~TcpServer() {
  delete handle_;
}
