#include "Socket.h"
#include <unistd.h>


Socket_t Socket(int domain, int type, int protocol) {
  Socket_t sd = socket(domain, type, protocol);
  if (sd == -1) {
    //FIXME : logging
    return -1;
  }
  return sd;
}

Socket_t SocketNoBlocking(int domain, int type, int protocol) {
  Socket_t socket = Socket(domain, type, protocol);
  if(socket != -1) {
    int flag = fcntl(socket, F_GETFL);
    int nflag = flag | O_NONBLOCK;
    if (-1 == fcntl(socket, F_SETFL, nflag) ){
      //FIXME : logging
      socket = -1;
    }
  }
  return socket;
}

bool SocketBind(Socket_t socket, NetAddress& addr) {
  int ret = ::bind(socket, (const struct sockaddr *)&addr.get_addr(), addr.get_addr_len());
  if (ret == -1) {
    //FIXME : logging
    return false;
  }
  return true;
}

bool SocketListen(Socket_t socket, int backlog) {
  int ret = ::listen(socket, backlog);
  if (ret == -1) {
    // FIXME : logging
    return false;
  }
  return true;
}

Socket_t SocketAccept(Socket_t socket, struct sockaddr* addr, socklen_t* addrlen) {
  int ret = ::accept(socket, addr, addrlen);
  if (ret == -1) {
    //FIXME : logging
    return ret;
  }
  return ret;
}
