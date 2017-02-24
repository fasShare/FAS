#include <TcpServer.h>
#include <iostream>
#include <Socket.h>
#include <unistd.h>
#include <boost/bind.hpp>
#include <EventLoop.h>
#include <Timestamp.h>

using namespace std;

TcpServer::TcpServer(EventLoop* loop,
                     const NetAddress& addr) :
  server_(SocketNoBlockingOrExec(AF_INET, SOCK_STREAM, 0)),
  loop_(loop),
  events_(server_, kReadEvent),
  handle_(new Handle(loop_, events_)),
  addr_(addr),
  listenBacklog_(50),
  threadPool_(loop, 4) {
  SocketBind(server_, addr_);
  SocketListen(server_, listenBacklog_);
}

EventLoop* TcpServer::getLoop() const{
  return loop_;
}

bool TcpServer::start() {
  handle_->setHandleRead(boost::bind(&TcpServer::handleReadEvent, this, _1, _2));
  loop_->addHandle(handle_);
  threadPool_.start();
  return true;
}

void TcpServer::handleReadEvent(Events* event, Timestamp time) {
    cout << time.toFormattedString() << " sd = " << event->getFd() << endl;

    int sd = ::accept(event->getFd(), NULL, NULL);

    close(sd);
}

TcpServer::~TcpServer() {
  delete handle_;
}
