#include <TcpConnection.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Socket.h>
#include <Log.h>
#include <Buffer.h>


#include <errno.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpConnection::TcpConnection(EventLoop *loop,
                                  const Events& event,
                                  const NetAddress& peerAddr,
                                  Timestamp now) :
  loop_(loop),
  event_(event),
  handle_(NULL),
  readBuffer_(new Buffer(1024)),
  writeBuffer_(new Buffer(1024)),
  connfd_(event.getFd()),
  peerAddr_(peerAddr),
  shouldBeClosed_(false),
  closeing_(false),
  acceptTime_(now),
  sendAllDataOut_(true) {
  assert(loop_ != NULL);
  handle_ = new Handle(loop_, event_);

  handle_->setHandleRead(boost::bind(&TcpConnection::handleRead, this, _1, _2));
  handle_->setHandleWrite(boost::bind(&TcpConnection::handleWrite, this, _1, _2));
  handle_->setHandleError(boost::bind(&TcpConnection::handleError, this, _1, _2));
  handle_->setHandleClose(boost::bind(&TcpConnection::handleClose, this, _1, _2));

  loop_->addHandle(handle_);

  LOGGER_DEBUG << "tid : " << gettid() <<  " TID : " << loop_->getTid() << fas::Log::CLRF;
}

fas::EventLoop* fas::TcpConnection::getLoop() {
  return loop_;
}

fas::Socket_t fas::TcpConnection::getConnfd() const {
  return connfd_.getSocket();
}

void fas::TcpConnection::setPeerAddr(const fas::NetAddress& addr) {
  peerAddr_ = addr;
}

void fas::TcpConnection::closeAndClearTcpConnection() {
  //Add this function to QueueInLoop can ensure handle_ destroy before TcpConnection
  LOGGER_TRACE << Log::CLRF;
  loop_->assertInOwnerThread();
  assert(closeing_);
  // FIXME : Other clear.
}

void fas::TcpConnection::setOnMessageCallBack(const fas::TcpConnMessageCallback& cb) {
  messageCb_ = cb;
}

bool fas::TcpConnection::messageCallbackVaild() {
  if (this->messageCb_) {
    return true;
  }
  return false;
}

void fas::TcpConnection::setOnCloseCallBack(const fas::CloseCallback& cb) {
  closeCb_ = cb;
}

void fas::TcpConnection::SetHasMoreDataCallback(HasMoreDataCallback moreDataCb) {
  moreDataCb_ = moreDataCb;
}

void fas::TcpConnection::setHasMoreData() {
  hasMoreData_ = true;
}

void fas::TcpConnection::unsetHasMoreData() {
  hasMoreData_ = false;
}

void fas::TcpConnection::sendString(const std::string& msg) {
  handle_->enableWrite();
  loop_->modHandle(handle_);
  putDataToWriteBuffer(msg.c_str(), msg.size());
}

void fas::TcpConnection::sendData(const void *data, size_t len) {
  handle_->enableWrite();
  loop_->modHandle(handle_);
  putDataToWriteBuffer(data, len);
}

void fas::TcpConnection::putDataToWriteBuffer(const void *data, size_t len) {
  this->sendAllDataOut_ = false;
  writeBuffer_->append(data, len);
}

void fas::TcpConnection::handleRead(const fas::Events& revents,
                                      fas::Timestamp time) {
  boost::ignore_unused(time);
  loop_->assertInOwnerThread();
  if (revents.getFd() != connfd_.getSocket()) {
    LOGGER_ERROR << revents.getFd() << " != " << connfd_.getSocket() << fas::Log::CLRF;
    if(!closeing_) {
      handleClose(revents, time);
    }
  }
  int err = 0;
  ssize_t ret = readBuffer_->readFd(revents.getFd(), &err);
  if (ret == 0) {
    if(!closeing_) {
      handleClose(revents, time);
    }
  } else if (ret < 0) {
    LOGGER_DEBUG << "readBuffer_.readFd return " << ::strerror(err) << Log::CLRF;
  } else {
    if (messageCb_) {
      messageCb_(shared_from_this(), readBuffer_, time);
    }
  }
}

void fas::TcpConnection::handleWrite(const fas::Events& revents, fas::Timestamp time) {
  boost::ignore_unused(revents, time);
  loop_->assertInOwnerThread();

  Socket_t writeSd = connfd_.getSocket();
  int readablesizes = writeBuffer_->readableBytes();
reWrite:
  int ret = ::write(writeSd, writeBuffer_->peek(), readablesizes);
  if (ret < 0) {
    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
      return;
    } else if (errno == EINTR) {
      goto reWrite;
    }
    LOGGER_SYSERR << "TcpConnection::handleWrite error" <<  ::strerror(errno) << Log::CLRF;
  } else if (ret == 0) {
    if(!closeing_) {
      sendAllDataOut_ = true;
      handleClose(revents, time);
    }
  } else {
    readablesizes -= ret;
    writeBuffer_->retrieve(ret);

    //It should be only used when you send mass data.
    if ((hasMoreData_ == true) && (moreDataCb_)) {
      moreDataCb_(shared_from_this());
      readablesizes = writeBuffer_->readableBytes();
    }

    if (readablesizes == 0) {
      sendAllDataOut_ = true;
      if (shouldBeClosed_) {
        handleClose(revents, time);
      } else {
        handle_->disableWrite();
        loop_->modHandle(handle_);
      }
    }
  }
}

void fas::TcpConnection::handleError(const fas::Events& revents,
                                       fas::Timestamp time) {
  LOGGER_TRACE << Log::CLRF;
  boost::ignore_unused(revents, time);
  loop_->assertInOwnerThread();
}

/*!
 * \brief fas::TcpConnection::handleClose
 * \param revents
 * \param time
 * close this TcpConnection immediately.
 */
void fas::TcpConnection::handleClose(const fas::Events& revents, fas::Timestamp time) {
  LOGGER_TRACE << Log::CLRF;
  boost::ignore_unused(revents, time);
  if (closeing_) {
    return;
  }
  loop_->delHandle(handle_);
  handle_ = nullptr;
  if(closeCb_) {
    closeing_ = true;
    closeCb_();
  }
  LOGGER_TRACE << " out" << Log::CLRF;
}
/*!
 * \brief fas::TcpConnection::shutdown
 * close this TcpConnection after buffer empty.
 */
void fas::TcpConnection::shutdown() {
  loop_->queueInLoop(boost::bind(shutdownFromThis, shared_from_this()));
}

void fas::shutdownFromThis(fas::TcpConnShreadPtr conn) {
  if (conn->closeing_) {
    return;
  }
  if (conn->sendAllDataOut_) {
    conn->getLoop()->delHandle(conn->handle_);
    if(conn->closeCb_) {
      conn->closeing_ = true;
      conn->closeCb_();
    }
  } else {
    conn->shouldBeClosed_ = true;
  }
}

fas::TcpConnection::~TcpConnection() {
  // don't use handle_ != nullptr replace with !closeing, think the reason, please!
  if (!closeing_) {
      loop_->delHandle(handle_);
  }
  delete readBuffer_;
  readBuffer_ = nullptr;
  delete writeBuffer_;
  writeBuffer_ = nullptr;
  LOGGER_TRACE << "tid: " << gettid() << " TcpConnection destroy!" << Log::CLRF;
}
