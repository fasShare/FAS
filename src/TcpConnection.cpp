#include <TcpConnection.h>
#include <Handle.h>
#include <Buffer.h>
#include <EventLoop.h>

TcpConnection::TcpConnection(EventLoop *loop,
                            const Events& event) :
  loop_(loop),
  event_(event),
  handle_(NULL),
  buffer_() {
  assert(loop_ != NULL);
  handle_ = new Handle(loop_, event_);
  loop_->addHandle(handle_);
}

EventLoop* TcpConnection::getLoop() {
  return loop_;
}

void TcpConnection::setHandleRead(const events_handle_t& handle_read) {
  handle_->setHandleRead(handle_read);
}

void TcpConnection::setHandleWrite(const events_handle_t& handle_write) {
  handle_->setHandleWrite(handle_write);
}

void TcpConnection::setHandleError(const events_handle_t& handle_error) {
  handle_->setHandleError(handle_error);
}

void TcpConnection::setHandleClose(const events_handle_t& handle_close) {
  handle_->setHandleClose(handle_close);
}

void TcpConnection::defaultHandleRead(Events* revents,
                                      Timestamp time) {
  loop_->assertInOwner();
  cout << "TcpConnection::defaultHandleRead" << endl;
  char buf[1024];
  int ret = ReadSocket(revents->getFd(), buf, 1023);
  buf[ret] = 0;
  cout << buf << endl;
}

void TcpConnection::defaultHandleWrite(Events* revents,
                                       Timestamp time) {
  loop_->assertInOwner();
}

void TcpConnection::defaultHandleError(Events* revents,
                                       Timestamp time) {
  loop_->assertInOwner();
}

void TcpConnection::defaultHandleClose(Events* revents,
                                       Timestamp time) {
  loop_->assertInOwner();
  close(revents->getFd());
  loop_->delHandle(handle_);
}
