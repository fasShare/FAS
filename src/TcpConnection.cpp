#include <TcpConnection.h>
#include <Handle.h>
#include <Buffer.h>
#include <EventLoop.h>

#include <boost/bind.hpp>

TcpConnection::TcpConnection(EventLoop *loop,
                            const Events& event) :
  loop_(loop),
  event_(event),
  handle_(NULL),
  buffer_(),
  connfd_(event.getFd()),
  closeing_(false) {
  assert(loop_ != NULL);
  handle_ = new Handle(loop_, event_);
  loop_->addHandle(handle_);

  handle_->setHandleRead(boost::bind(&TcpConnection::handleRead, this, _1, _2));
  handle_->setHandleWrite(boost::bind(&TcpConnection::handleWrite, this, _1, _2));
  handle_->setHandleError(boost::bind(&TcpConnection::handleError, this, _1, _2));
  handle_->setHandleClose(boost::bind(&TcpConnection::handleClose, this, _1, _2));
}

EventLoop* TcpConnection::getLoop() {
  return loop_;
}

int TcpConnection::getConnfd() const {
  return connfd_;
}

void TcpConnection::closeAndClearTcpConnection() {
  loop_->delHandle(handle_);
  cout << __func__ << endl;
}

void TcpConnection::setOnMessageCallBack(const MessageCallback& cb) {
  messageCb_ = cb;
}

void TcpConnection::setOnCloseCallBack(const CloseCallback& cb) {
  closeCb_ = cb;
}

void TcpConnection::handleRead(Events revents,
                                      Timestamp time) {
  loop_->assertInOwner();
  //cout << "TcpConnection::defaultHandleRead" << endl;
  char buf[1024];
  int ret = ReadSocket(revents.getFd(), buf, 1023);
  if (ret == 0) {
    if(!closeing_) {
      cout << "ret == 0 fd = " << revents.getFd() << endl;
      handleClose(revents, time);
    }
  } else if (ret < 0) {

  } else {
    buf[ret] = 0;
    cout << buf << endl;
     //messageCb_();
  }
}

void TcpConnection::handleWrite(Events revents,
                                       Timestamp time) {
  loop_->assertInOwner();
}

void TcpConnection::handleError(Events revents,
                                       Timestamp time) {
  loop_->assertInOwner();
}

void TcpConnection::handleClose(Events revents, Timestamp time) {
  assert(!closeing_);
  if(closeCb_) {
    closeCb_();
  }
  closeing_ = true;
}

