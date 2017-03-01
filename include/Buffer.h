#ifndef FAS_BUFFER_H
#define FAS_BUFFER_H
#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>


#include <Endian.h>


#include <boost/noncopyable.hpp>

using std::string;

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer : boost::noncopyable {
public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 100;

  Buffer();

  // default copy-ctor, dtor and assignment are fine

  void swap(Buffer& rhs);

  size_t readableBytes() const;
  size_t writableBytes() const;
  size_t prependableBytes() const;

  const char* peek() const;

  const char* findCRLF() const;
  const char* findCRLF(const char* start) const;

  // retrieve returns void, to prevent
  // string str(retrieve(readableBytes()), readableBytes());
  // the evaluation of two functions are unspecified
  void retrieve(size_t len);
  void retrieveUntil(const char* end);
  void retrieveInt32();
  void retrieveInt16();
  void retrieveInt8();
  void retrieveAll();

  string retrieveAllAsString();
  string retrieveAsString(size_t len);


  void append(const char* /*restrict*/ data, size_t len);
  void append(const void* /*restrict*/ data, size_t len);

  void ensureWritableBytes(size_t len);
  char* beginWrite();
  const char* beginWrite() const;
  void hasWritten(size_t len);

  ///
  /// Append int32_t using network endian
  ///
  void appendInt32(int32_t x);
  void appendInt16(int16_t x);
  void appendInt8(int8_t x);
  ///
  /// Read int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t readInt32();
  int16_t readInt16();
  int8_t readInt8();
  ///
  /// Peek int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t peekInt32() const;
  int16_t peekInt16() const;
  int8_t peekInt8() const;

  ///
  /// Prepend int32_t using network endian
  ///
  void prependInt32(int32_t x);
  void prependInt16(int16_t x);
  void prependInt8(int8_t x);
  void prepend(const void* /*restrict*/ data, size_t len);

  /// Read data directly into buffer.
  ///
  /// It may implement with readv(2)
  /// @return result of read(2), @c errno is saved
  ssize_t readFd(int fd, int* savedErrno);

private:

  char* begin();
  const char* begin() const;

  void makeSpace(size_t len);

private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;

  static const char kCRLF[];
};
#endif  // FAS_BUFFER_H
