#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <memory>


#include <Types.h>
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>

namespace fas {

class Handle;
class EventLoop;
class buffer;
/*!
 * \brief The TcpConnection class
 * The TcpConnection class was created by TcpServer.
 * It's connfd_ connected to the client.
 */
class TcpConnection {
public:
  TcpConnection(EventLoop *loop,
                const Events& event,
                const NetAddress& peerAddr_,
                Timestamp now = Timestamp::now());
  ~TcpConnection();
  /*!
   * \brief getLoop
   * \return EventLoop*
   * IO loop (workLoop), the handle of this TcpConnection was polling on it.
   */
  EventLoop* getLoop();
  /*!
   * \brief getConnfd
   * \return Socket_t(int)
   * connfd_ connected to the client
   */
  Socket_t getConnfd() const;

  void setPeerAddr(const NetAddress& addr);
  /*!
   * \brief closeAndClearTcpConnection
   * This function will be called when this TcpConnection destroyed.
   */
  void closeAndClearTcpConnection();

  void setOnMessageCallBack(const MessageCallback& cb);
  void setOnCloseCallBack(const CloseCallback& cb);
  /*!
   * \brief sendString
   * \param msg
   * \return size_t
   * \see sendData
   * send msg to connfd_'s peer socket.
   */
  void sendString(const std::string& msg);
  void sendData(const void *data, size_t len);
  /*!
   * \brief handleRead
   * \param revents
   * \param time
   * \see handleWrite, handleError, handleClose
   * The default events hadnle function.
   * It'll be boost::bind to handle_.
   */
  void handleRead(const Events& revents, Timestamp time);
  void handleWrite(const Events& revents, Timestamp time);
  void handleError(const Events& revents, Timestamp time);
  void handleClose(const Events& revents, Timestamp time);

private:
  EventLoop *loop_;
  Events event_;        /*!< events of connfd_ */
  Handle *handle_;
  Buffer *readBuffer_;
  Buffer *writeBuffer_;
  Socket connfd_;      /*!< connected socket. */
  NetAddress peerAddr_;
  bool closeing_;
  CloseCallback closeCb_;
  MessageCallback messageCb_;
  Timestamp acceptTime_;
};

}
#endif // FAS_TCPCONNECTION_H

