#ifndef FAS_TYPES_H
#define FAS_TYPES_H
#include <sys/epoll.h>
#include <sys/poll.h>

typedef struct epoll_event  EpollEvent;
typedef struct pollfd  PollEvent;

typedef int Socket_t;
typedef int events_t;

typedef unsigned char uchar;
typedef unsigned int uint;

#endif // FAS_TYPES_H

