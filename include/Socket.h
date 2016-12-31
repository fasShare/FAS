#ifndef SOCKET_H
#define SOCKET_H
#include "NetAddress.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

class Socket
{
private:
  int sd;
public:
  Socket(int socket);
  Socket(int domain, int type, int protocol);
  Socket(int domain, int type);
  bool Socket_bind(NetAddress &addr);
  bool Socket_bind(const struct sockaddr *addr,
                socklen_t addrlen);
  bool Socket_listen(int backlog);
  int Socket_get_sd();
  void Socket_close();
};

#endif // SOCKET_H
