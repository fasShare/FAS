#include "Socket.h"
#include <unistd.h>

Socket::Socket(int socket):
    sd(socket) {
  int flag = fcntl(sd, F_GETFL);
  int nflag = flag | O_NONBLOCK;
  fcntl(sd, F_SETFL, nflag);
}

Socket::Socket(int domain, int type, int protocol) {
  //FIXME : IF ANY ERROR
  sd = socket(domain, type, protocol);
}

Socket::Socket(int domain, int type):
  Socket(domain, type, 0){
}

bool Socket::Socket_bind(NetAddress &addr) {
  int ret = bind(sd, (const struct sockaddr *)&addr.NetAddress_get_addr(), \
                 addr.NetAddress_get_addr_len());
  return ret == -1 ? false : true;
}

bool Socket::Socket_listen(int backlog) {
  int ret = listen(sd, backlog);
  return ret == -1 ? false : true;
}

bool Socket::Socket_bind(const struct sockaddr *addr,
                        socklen_t addrlen) {
  int ret = bind(sd, addr, addrlen);
  return ret == -1 ? false : true;
}

int Socket::Socket_get_sd() {
  return sd;
}

void Socket::Socket_close() {
  close(sd);
}
