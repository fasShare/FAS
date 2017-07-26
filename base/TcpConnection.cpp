#include <TcpConnection.h>
#include <Handle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Socket.h>
#include <Thread.h>
#include <Log.h>
#include <Buffer.h>


#include <errno.h>


#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpConnection::TcpConnection(EventLoop *loop, const Events& event, const NetAddress& peerAddr, Timestamp now) :
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

    LOGGER_DEBUG("tid : " << gettid() <<  " TID : " << loop_->getTid());
}

fas::EventLoop* fas::TcpConnection::getLoop() {
    return loop_;
}

int fas::TcpConnection::getConnfd() const {
    return connfd_.getSocket();
}

void fas::TcpConnection::setPeerAddr(const fas::NetAddress& addr) {
    peerAddr_ = addr;
}

void fas::TcpConnection::setOnMessageCallBack(const TcpConnMessageCallback& cb) {
    messageCb_ = cb;
}

bool fas::TcpConnection::messageCallbackVaild() {
    if (this->messageCb_) {
        return true;
    }
    return false;
}

void fas::TcpConnection::setOnCloseCallBack(const CloseCallback& cb) {
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
        LOGGER_ERROR(revents.getFd() << " != " << connfd_.getSocket());
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
        LOGGER_DEBUG("readBuffer_.readFd return -1 : " << ::strerror(err));
    } else {
        if (messageCb_) {
            messageCb_(shared_from_this(), readBuffer_, time);
        }
    }
}

void fas::TcpConnection::handleWrite(const fas::Events& revents, fas::Timestamp time) {
    boost::ignore_unused(revents, time);
    loop_->assertInOwnerThread();

    int writeSd = connfd_.getSocket();
    size_t readablesizes = writeBuffer_->readableBytes();
reWrite:
    ssize_t ret = ::write(writeSd, writeBuffer_->peek(), readablesizes);
    if (ret < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            return;
        } else if (errno == EINTR) {
            goto reWrite;
        }
        LOGGER_SYSERR("TcpConnection::handleWrite error" <<  ::strerror(errno));
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

void fas::TcpConnection::handleError(const fas::Events& revents, fas::Timestamp time) {
    LOGGER_TRACE("handleError");
    boost::ignore_unused(revents, time);
    loop_->assertInOwnerThread();
}

void fas::TcpConnection::handleClose(const fas::Events& revents, fas::Timestamp time) {
    LOGGER_TRACE("TcpConnection::handleClose");
    // don't send the remainder data
	sendAllDataOut_ = true;
    shutdown();
    LOGGER_TRACE("TcpConnection::handleClose out");
}

void fas::TcpConnection::shutdown() {
    if (closeing_) {
        return;
    }
    if (sendAllDataOut_) {
        loop_->delHandle(handle_);
        if(closeCb_) {
            closeing_ = true;
            closeCb_();
        }
    } else {
        shouldBeClosed_ = true;
    }
}

void fas::TcpConnection::clearInLoop() {
    LOGGER_TRACE("TcpConnection::closeAndClearTcpConnection");
    loop_->assertInOwnerThread();
    assert(closeing_);
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
    LOGGER_TRACE("tid: " << gettid() << " TcpConnection destroy!");
}
