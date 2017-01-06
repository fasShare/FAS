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
  bool available;

  Socket& operator= (const Socket& sd);
public:
  Socket(int socket);
  Socket(int domain, int type, int protocol);
  Socket(int domain, int type);
  Socket() {

  }


  Socket& operator= (const int sd);

  bool Socket_set_noblock();

  bool Socket_bind(NetAddress &addr);
  bool Socket_bind(const struct sockaddr *addr,
                socklen_t addrlen);

  bool Socket_listen(int backlog);

  Socket Socket_accept();

  void Socket_set_available(bool boolean);
  bool Socket_get_available();

	int Socket_accept(struct sockaddr* addr, socklen_t* addrlen);

  int Socket_get_sd();

  void Socket_close();
};

#endif // SOCKET_H
