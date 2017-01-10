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

  bool set_noblock();

  bool bind(NetAddress &addr);
  bool bind(const struct sockaddr *addr,
                socklen_t addrlen);

  bool listen(int backlog);

  Socket accept();

  void set_available(bool boolean);
  bool get_available();

    int accept(struct sockaddr* addr, socklen_t* addrlen);

  int get_sd();

  void close();
};

#endif // SOCKET_H
