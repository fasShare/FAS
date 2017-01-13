#include <Buffer.h>
#include <Socket.h>

#include <errno.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <boost/implicit_cast.hpp>


const char Buffer::kCRLF[] = "\r\n";
const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

Buffer::Buffer()
  : buffer_(kCheapPrepend + kInitialSize),
    readerIndex_(kCheapPrepend),
    writerIndex_(kCheapPrepend)
{
  assert(readableBytes() == 0);
  assert(writableBytes() == kInitialSize);
  assert(prependableBytes() == kCheapPrepend);
}

void Buffer::swap(Buffer& rhs) {
  buffer_.swap(rhs.buffer_);
  std::swap(readerIndex_, rhs.readerIndex_);
  std::swap(writerIndex_, rhs.writerIndex_);
}

size_t Buffer::readableBytes() const {
  return writerIndex_ - readerIndex_;
}

size_t Buffer::writableBytes() const {
  return buffer_.size() - writerIndex_;
}

size_t Buffer::prependableBytes() const {
  return readerIndex_;
}

const char* Buffer::peek() const {
  return begin() + readerIndex_;
}

const char* Buffer::findCRLF() const {
  const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
  return crlf == beginWrite() ? NULL : crlf;
}

const char* Buffer::findCRLF(const char* start) const {
  assert(peek() <= start);
  assert(start <= beginWrite());
  const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
  return crlf == beginWrite() ? NULL : crlf;
}

// retrieve returns void, to prevent
// string str(retrieve(readableBytes()), readableBytes());
// the evaluation of two functions are unspecified
void Buffer::retrieve(size_t len) {
  assert(len <= readableBytes());
  if (len < readableBytes()) {
    readerIndex_ += len;
  }
  else {
    retrieveAll();
  }
}

void Buffer::retrieveUntil(const char* end) {
  assert(peek() <= end);
  assert(end <= beginWrite());
  retrieve(end - peek());
}

void Buffer::retrieveInt32() {
  retrieve(sizeof(int32_t));
}

void Buffer::retrieveInt16() {
  retrieve(sizeof(int16_t));
}

void Buffer::retrieveInt8() {
  retrieve(sizeof(int8_t));
}

void Buffer::retrieveAll() {
  readerIndex_ = kCheapPrepend;
  writerIndex_ = kCheapPrepend;
}

string Buffer::retrieveAllAsString() {
  return retrieveAsString(readableBytes());;
}

string Buffer::retrieveAsString(size_t len) {
  assert(len <= readableBytes());
  string result(peek(), len);
  retrieve(len);
  return result;
}


void Buffer::append(const char* /*restrict*/ data, size_t len) {
  ensureWritableBytes(len);
  std::copy(data, data+len, beginWrite());
  hasWritten(len);
}

void Buffer::append(const void* /*restrict*/ data, size_t len) {
  append(static_cast<const char*>(data), len);
}

void Buffer::ensureWritableBytes(size_t len) {
  if (writableBytes() < len) {
    makeSpace(len);
  }
  assert(writableBytes() >= len);
}

char* Buffer::beginWrite() {
  return begin() + writerIndex_;
}

const char* Buffer::beginWrite() const {
  return begin() + writerIndex_;
}

void Buffer::hasWritten(size_t len) {
  writerIndex_ += len;
}

///
/// Append int32_t using network endian
///
void Buffer::appendInt32(int32_t x) {
  int32_t be32 = sockets::hostToNetwork32(x);
  append(&be32, sizeof be32);
}

void Buffer::appendInt16(int16_t x) {
  int16_t be16 = sockets::hostToNetwork16(x);
  append(&be16, sizeof be16);
}

void Buffer::appendInt8(int8_t x) {
  append(&x, sizeof x);
}

///
/// Read int32_t from network endian
///
/// Require: buf->readableBytes() >= sizeof(int32_t)
int32_t Buffer::readInt32() {
  int32_t result = peekInt32();
  retrieveInt32();
  return result;
}

int16_t Buffer::readInt16() {
  int16_t result = peekInt16();
  retrieveInt16();
  return result;
}

int8_t Buffer::readInt8() {
  int8_t result = peekInt8();
  retrieveInt8();
  return result;
}

///
/// Peek int32_t from network endian
///
/// Require: buf->readableBytes() >= sizeof(int32_t)
int32_t Buffer::peekInt32() const {
  assert(readableBytes() >= sizeof(int32_t));
  int32_t be32 = 0;
  ::memcpy(&be32, peek(), sizeof be32);
  return sockets::networkToHost32(be32);
}

int16_t Buffer::peekInt16() const {
  assert(readableBytes() >= sizeof(int16_t));
  int16_t be16 = 0;
  ::memcpy(&be16, peek(), sizeof be16);
  return sockets::networkToHost16(be16);
}

int8_t Buffer::peekInt8() const {
  assert(readableBytes() >= sizeof(int8_t));
  int8_t x = *peek();
  return x;
}

///
/// Prepend int32_t using network endian
///
void Buffer::prependInt32(int32_t x) {
  int32_t be32 = sockets::hostToNetwork32(x);
  prepend(&be32, sizeof be32);
}

void Buffer::prependInt16(int16_t x) {
  int16_t be16 = sockets::hostToNetwork16(x);
  prepend(&be16, sizeof be16);
}

void Buffer::prependInt8(int8_t x) {
  prepend(&x, sizeof x);
}

void Buffer::prepend(const void* /*restrict*/ data, size_t len) {
  assert(len <= prependableBytes());
  readerIndex_ -= len;
  const char* d = static_cast<const char*>(data);
  std::copy(d, d+len, begin()+readerIndex_);
}

ssize_t Buffer::readFd(int fd, int* savedErrno) {
  // saved an ioctl()/FIONREAD call to tell how much to read
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = writableBytes();
  vec[0].iov_base = begin()+writerIndex_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  const ssize_t n = ReadvSocket(fd, vec, 2);
  if (n < 0) {
    *savedErrno = errno;
  }
  else if (boost::implicit_cast<size_t>(n) <= writable) {
    writerIndex_ += n;
  }
  else {
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable);
  }
  // if (n == writable + sizeof extrabuf)
  // {
  //   goto line_30;
  // }
  return n;
}

char* Buffer::begin() {
  return &*buffer_.begin();
}

const char* Buffer::begin() const {
  return &*buffer_.begin();
}

void Buffer::makeSpace(size_t len) {    //
  if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
    // FIXME: move readable data
    buffer_.resize(writerIndex_+len);
  }
  else {
    // move readable data to the front, make space inside buffer
    assert(kCheapPrepend < readerIndex_);
    size_t readable = readableBytes();
    std::copy(begin()+readerIndex_,
              begin()+writerIndex_,
              begin()+kCheapPrepend);
    readerIndex_ = kCheapPrepend;
    writerIndex_ = readerIndex_ + readable;
    assert(readable == readableBytes());
  }
}


