#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <memory>
#include <map>
#include <vector>


#include <Socket.h>
#include <Types.h>
#include <Events.h>
#include <NetAddress.h>
#include <EventLoopThreadPool.h>
#include <TcpConnection.h>

namespace fas {

class EventLoop;
class Handle;
/*!
 * \brief The TcpServer class
 * The TcpServer class main job is manage TcpConnection and Handle.
 * TcpServer create TcpConnection in it's listenning socket's handread callback,
 * then established the relationship between TcpConnection and Handle,
 * The callbacks of TcpConnection's Handle was seted by TcpSerevr, then TcpServer
 * add TcpConnection's Handle to Io Thread's loop(workloop) and wake up this Io thread from polling.
 */
class TcpServer {
public:
  TcpServer(EventLoop *loop, const NetAddress& addr, uint threadNum = 4);
  ~TcpServer();

  EventLoop* getLoop() const;

  TcpConnShreadPtr getConn(map_conn_key_t key) const;
  TcpConnShreadPtr getConn(map_conn_key_t key);

  bool start();
  /*!
   * \brief handleReadEvent
   * \param event
   * \param time
   * This function is accept socket's callback.
   * when a new client connect to this TcpServer, it'll be called.
   * We create new TcpConnection in it.
   */
  void handleReadEvent(const Events& event, Timestamp time);
  void setOnConnectionCallBack(OnConnectionCallBack onConnectionCb);
  void setOnConnRemovedCallBack(OnConnectionRemovedCallBack onConnRemovedCb);
  /*!
   * \brief setMessageCallback
   * \param cb
   * This function set the message's callback of TcpConnection was created in
   * function TcpServer::handleReadEvent.
   * The MessageCallback will be called when TcpConnection's connected socket can be read, there are
   * a message come.
   */
  void setMessageCallback(const MessageCallback& cb);
  /*!
   * \brief removeConnection
   * \param conn
   * \see removeConnectionInLoop
   * When TcpConnection's will be closed this function will be called.
   */
  void removeConnection(map_conn_key_t conn);
  void removeConnectionInLoop(map_conn_key_t conn);
private:
  Socket server_;                     /*!< listen and accept socket. */
  EventLoop *loop_;                   /*!< In loop_ polling server_'s read and write events. */
  Events events_;
  Handle *handle_;                    /*!< Handle of Socket. */
  NetAddress addr_;
  const uint listenBacklog_;
  EventLoopThreadPool threadPool_;    /*!< New TcpConnection's handle will be added
                                      to EventLoopThread in this Threadpool. */
  std::map<int, std::shared_ptr<TcpConnection> > conns_;  /*!< map of socket and TcpConnection. */
  OnConnectionCallBack onConnectionCb_;
  OnConnectionRemovedCallBack onConnRemovedCb_;
  MessageCallback messageCb_;   /*!< Callback for TcpConnection. */
};

}
#endif // FAS_TCPSERVER_H
