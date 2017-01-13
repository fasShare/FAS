#ifndef NETBASETYPES_H
#define NETBASETYPES_H
#include <sys/epoll.h>
#include <sys/poll.h>

typedef struct epoll_event  Epoll_Event;
typedef struct pollfd  Poll_Event;

typedef int Socket_t;

typedef int events_t;

#endif // NETBASETYPES_H

