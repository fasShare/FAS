#ifndef FAS_BUFFER_H
#define FAS_BUFFER_H
#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>


#include <Endian.h>


#include <boost/noncopyable.hpp>

using std::string;

/*!
 * A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
 * @code
 *  +-------------------+------------------+------------------+
 *  | prependable bytes |  readable bytes  |  writable bytes  |
 *  |                   |     (CONTENT)    |                  |
 *  +-------------------+------------------+------------------+
 *  |                   |                  |                  |
 *  0      <=      readerIndex   <=   writerIndex    <=     size
 *  @endcode
 */
class Buffer : boost::noncopyable {
public:
  static const size_t kCheapPrepend = 8;

  Buffer(size_t initialSize_);

  /*!
   * \brief swap
   * \param rhs
   * The buffer_, readerIndex_ and writerIndex_ of this Buffer and rhs all will be swapped.
   */
  void swap(Buffer& rhs);
  /*!
   * \brief readableBytes
   * \return size_t
   * Get the bytes can read from this buffer.
   */
  size_t readableBytes() const;

  /*!
   * \brief writableBytes
   * \return size_t
   * Get the bytes can writen to this buffer without invoking makeSpace(len);
   */
  size_t writableBytes() const;

  /*!
   * \brief prependableBytes
   * \return size_t
   * Get the bytes has been read out, those space can be used to store new data.
   */
  size_t prependableBytes() const;

  /*!
   * \brief peek
   * \return const char *
   * Gte the start address of the data can be read
   */
  const char* peek() const;

  /*!
   * \brief findCRLF
   * \return const char *
   * The function can be use to find CRLF in http header.
   */
  const char* findCRLF() const;
  const char* findCRLF(const char* start) const;

  /*!
   * \brief retrieve
   * \param len
   * The len bytes of readableBytes() will can't be use for read.
   * retrieve returns void, to prevent string str(retrieve(readableBytes()), readableBytes());
   * the evaluation of two functions are unspecified
   */
  void retrieve(size_t len);
  void retrieveUntil(const char* end);
  void retrieveInt32();
  void retrieveInt16();
  void retrieveInt8();
  void retrieveAll();

  /*!
   * \brief retrieveAllAsString
   * \return string
   * \see retrieveAsString
   * Get all the data as string,the data returned can not be read.
   */
  string retrieveAllAsString();
  string retrieveAsString(size_t len);

  /*!
   * \brief append
   * \param data
   * \param len
   * Add len bytes data to the space can be write of buffer,
   * if writableBytes() < len the makeSpace(len) will be called
   */
  void append(const char* /*restrict*/ data, size_t len);
  void append(const void* /*restrict*/ data, size_t len);
  /*!
   * \brief ensureWritableBytes
   * \param len
   * This funtion will be call makeSpace to make enough space to make sure writableBytes() > len
   */
  void ensureWritableBytes(size_t len);
  /*!
   * \brief beginWrite
   * \return char *
   * Get the start address can write data.
   */
  char* beginWrite();
  const char* beginWrite() const;
  /*!
   * \brief hasWritten
   * \param len
   * Update writableBytes = writableBytes - len
   */
  void hasWritten(size_t len);

  /*!
   * \brief appendInt32
   * \param x
   * \see appendInt16, appendInt8
   * Append int32_t x using network endian
   */
  void appendInt32(int32_t x);
  void appendInt16(int16_t x);
  void appendInt8(int8_t x);
  /*!
   * \brief readInt32
   * \return int32_t
   * \see readInt16, readInt8
   * Read int32_t from network endian,
   * Require buf->readableBytes() >= sizeof(int32_t)
   */
  int32_t readInt32();
  int16_t readInt16();
  int8_t readInt8();

  /*!
   * \brief peekInt32
   * \return int32_t
   * \see peekInt16, peekInt8
   * Peek int32_t from network endian
   * Require: buf->readableBytes() >= sizeof(int32_t)
   */
  int32_t peekInt32() const;
  int16_t peekInt16() const;
  int8_t peekInt8() const;

  /*!
   * \brief prependInt32
   * \param x
   * \see prependInt16, prependInt8, prepend
   * Prepend int32_t x using network endian
   */
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
  /*!
   * \brief begin
   * \return char *
   * Get the start address of buffer
   */
  char* begin();
  const char* begin() const;
  /*!
   * \brief makeSpace
   * \param len
   * \see prependableBytes
   * If the space has been read and the space can be write can't contain len bytes data
   * the buffer_ will be resize(writerIndex_+len).
   * else the data will be copy to the space prependableBytes.
   */
  void makeSpace(size_t len);

private:
  size_t initialSize_;
  std::vector<char> buffer_;  /*!< data buffer */
  size_t readerIndex_;        /*!< buffer.begin() + readerIndex_ == peek()
                                   writerIndex_ - readerIndex_ == readableBytes()
                               */
  size_t writerIndex_;        /*!< buffer.begin() + writerIndex_ == beginWrite()
                                   buffer_.size() - writerIndex_ == writableBytes()
                               */

  static const char kCRLF[];
};
#endif  // FAS_BUFFER_H
