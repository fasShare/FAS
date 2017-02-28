#ifndef FAS_TYPES_H
#define FAS_TYPES_H
#include <sys/epoll.h>
#include <sys/poll.h>


#include <boost/function.hpp>

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

typedef std::shared_ptr<TcpConnection> TcpConnShreadPtr;
typedef boost::function<void (Events, Timestamp)> events_handle_t;


typedef int map_conn_key_t;

typedef boost::function<void (map_conn_key_t)> CloseCallback;
// the data has been read to (buf, len)
typedef boost::function<void (const TcpConnection *,
                              Buffer*,
                              Timestamp)> MessageCallback;
typedef boost::function<bool (const Events&)> EventCheckFunc;

template<typename T>
std::shared_ptr<T> getSharedPtr(T *ptr) {
  return std::shared_ptr<T>(ptr);
}

class Timer;
typedef int timerfd_t;
typedef std::shared_ptr<Timer> TimerPtr;

#endif // FAS_TYPES_H

