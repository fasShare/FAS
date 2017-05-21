#ifndef FAS_TYPES_H
#define FAS_TYPES_H
#include <sys/epoll.h>
#include <sys/poll.h>


#include <boost/function.hpp>

namespace fas {

typedef struct epoll_event  EpollEvent;
typedef struct pollfd  PollEvent;

typedef int Socket_t;
typedef int events_t;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ubyte;

class Events;
class Timestamp;
class TcpConnection;
class Buffer;

typedef boost::function<void (Events, Timestamp)> events_handle_t;

typedef int map_conn_key_t;





typedef TcpConnection *TcpConnectionPtr;
typedef std::shared_ptr<TcpConnection> TcpConnShreadPtr;
typedef std::pair<int, TcpConnectionPtr> connkey_t;
typedef boost::function<void (TcpConnShreadPtr)> OnConnectionCallBack;
typedef boost::function<void (connkey_t)> OnConnectionRemovedCallBack;
typedef boost::function<void ()> CloseCallback;

// the data has been read to (buf, len)
typedef boost::function<void (TcpConnection *,
                              Buffer*,
                              Timestamp)> MessageCallback;
typedef boost::function<void (TcpConnection *)> HasMoreDataCallback;
typedef boost::function<bool (const Events&)> EventCheckFunc;

template<typename T>
std::shared_ptr<T> getSharedPtr(T *ptr) {
  return std::shared_ptr<T>(ptr);
}

class Timer;
typedef int timerfd_t;
typedef std::shared_ptr<Timer> TimerPtr;

}
#endif // FAS_TYPES_H

