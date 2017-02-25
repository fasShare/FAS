#ifndef FAS_SOCKET_H
#define FAS_SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>



#include <Default.h>


class NetAddress;

Socket_t Socket(int domain, int type, int protocol);
Socket_t SocketNoBlockingOrExec(int domain, int type, int protocol);

bool SetNoBlockingOrExec(Socket_t sd);

bool SocketBind(Socket_t socket, NetAddress& addr);

bool SocketListen(Socket_t socket, int backlog);

Socket_t SocketAccept(Socket_t socket, struct sockaddr* addr, socklen_t* addrlen);

ssize_t ReadSocket(int sockfd, void *buf, size_t count);

ssize_t ReadvSocket(int sockfd, const struct iovec *iov, int iovcnt);

ssize_t WriteSocket(int sockfd, const void *buf, size_t count);

void CloseSocket(int sockfd);

void ShutdownWrite(int sockfd);

#endif // FAS_SOCKET_H
