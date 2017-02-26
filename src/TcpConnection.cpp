#include <TcpConnection.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Socket.h>
#include <Log.h>
#include <Buffer.h>


#include <memory>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

void TcpConnMessageCallback(const TcpConnection *conn, Buffer *buffer, \
                            Timestamp time) {
  std::cout << time.toFormattedString() << " from socket : " \
            << conn->getConnfd() <<" recv : " \
            << buffer->retrieveAllAsString() << std::endl;
}

TcpConnection::TcpConnection(EventLoop *loop,
                            const Events& event) :
  loop_(loop),
  event_(event),
  handle_(NULL),
  buffer_(new Buffer()),
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
  loop_->assertInOwnerThread();
  assert(closeing_);
  loop_->delHandle(handle_);
}

void TcpConnection::setOnMessageCallBack(const MessageCallback& cb) {
  messageCb_ = cb;
}

void TcpConnection::setOnCloseCallBack(const CloseCallback& cb) {
  closeCb_ = cb;
}

void TcpConnection::handleRead(Events revents,
                                      Timestamp time) {
  boost::ignore_unused(time);
  loop_->assertInOwnerThread();
  int err = 0;
  //ssize_t ret = ReadvSocket(revents.getFd(), buf, 1023);
  ssize_t ret = buffer_->readFd(revents.getFd(), &err);
  if (ret == 0) {
    if(!closeing_) {
      handleClose(revents, time);
    }
  } else if (ret < 0) {
    LOG_DEBUG(std::string("buffer_.readFd return ") + ::strerror(err));
  } else {
    if (messageCb_) {
      messageCb_(this, buffer_, time);
    }
  }
}

void TcpConnection::handleWrite(Events revents,
                                       Timestamp time) {
  boost::ignore_unused(revents, time);
  loop_->assertInOwnerThread();
}

void TcpConnection::handleError(Events revents,
                                       Timestamp time) {
  boost::ignore_unused(revents, time);
  loop_->assertInOwnerThread();
}

void TcpConnection::handleClose(Events revents, Timestamp time) {
  boost::ignore_unused(revents, time);
  assert(!closeing_);
  if(closeCb_) {
    closeCb_();
  }
  closeing_ = true;
}

