#ifndef SOCKET_H
#define SOCKET_H
#include <NetAddress.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <NetBaseTypes.h>


Socket_t Socket(int domain, int type, int protocol);
Socket_t SocketNoBlocking(int domain, int type, int protocol);

bool SocketBind(Socket_t socket, NetAddress& addr);

bool SocketListen(Socket_t socket, int backlog);

Socket_t SocketAccept(Socket_t socket, struct sockaddr* addr, socklen_t* addrlen);

#endif // SOCKET_H
