#include <TcpConnection.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Socket.h>
#include <Log.h>
#include <Buffer.h>


#include <memory>
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
  loop_->addHandle(handle_);

  handle_->setHandleRead(boost::bind(&TcpConnection::handleRead, this, _1, _2));
  handle_->setHandleWrite(boost::bind(&TcpConnection::handleWrite, this, _1, _2));
  handle_->setHandleError(boost::bind(&TcpConnection::handleError, this, _1, _2));
  handle_->setHandleClose(boost::bind(&TcpConnection::handleClose, this, _1, _2));
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
  loop_->assertInOwnerThread();
  assert(closeing_);
  // FIXME : Other clear.
}

void fas::TcpConnection::setOnMessageCallBack(const fas::MessageCallback& cb) {
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
  writeBuffer_->append(data, len);
}

void fas::TcpConnection::handleRead(const fas::Events& revents,
                                      fas::Timestamp time) {
  boost::ignore_unused(time);
  loop_->assertInOwnerThread();
  assert(revents.getFd() == connfd_.getSocket());
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
      // FIXME : replace this with share_ptr
      messageCb_(this, readBuffer_, time);
    }
  }
}

void fas::TcpConnection::handleWrite(const fas::Events& revents, fas::Timestamp time) {
//  LOGGER_TRACE << "TcpConnection::handleWrite" << fas::Log::CLRF;
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
    sendAllDataOut_ = false;
    readablesizes -= ret;
    writeBuffer_->retrieve(ret);

    //It should be only used when you send mass data.
    if (hasMoreData_ == true) {
      assert(moreDataCb_);
      moreDataCb_(this);
      readablesizes = writeBuffer_->readableBytes();
    }

    if (readablesizes == 0) {
      sendAllDataOut_ = true;
      if (shouldBeClosed_) {
        handleClose(revents, time);
      }
      handle_->disableWrite();
      loop_->modHandle(handle_);
    }
  }
//  LOGGER_TRACE << "TcpConnection::handleWrite" << fas::Log::CLRF;
}

void fas::TcpConnection::handleError(const fas::Events& revents,
                                       fas::Timestamp time) {
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
  boost::ignore_unused(revents, time);
  assert(!closeing_);
  loop_->delHandle(handle_);
  if(closeCb_) {
    closeing_ = true;
    closeCb_(connfd_.getSocket());
  }
}
/*!
 * \brief fas::TcpConnection::shutdown
 * close this TcpConnection after buffer empty.
 */
void fas::TcpConnection::shutdown() {
  if (this->sendAllDataOut_) {
    loop_->delHandle(handle_);
    if(closeCb_) {
      closeing_ = true;
      closeCb_(connfd_.getSocket());
    }
  } else {
    shouldBeClosed_ = true;
  }
}

fas::TcpConnection::~TcpConnection() {
  LOGGER_TRACE << "TcpConnection destroy!" << Log::CLRF;
  if (handle_->getState() != Handle::STATE_DEL) {
    loop_->delHandle(handle_);
  }
  delete readBuffer_;
}
