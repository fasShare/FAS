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

Socket_t SocketNoBlockingOrExec(int domain, int type, int protocol) {
  Socket_t socket = Socket(domain, type, protocol);
  if(socket != -1) {
    if (false == SetNoBlockingOrExec(socket) ){
      //FIXME : logging
      socket = -1;
    }
  }
  return socket;
}


bool SetNoBlockingOrExec(Socket_t sd) {
  int flag = fcntl(sd, F_GETFL);
  int nflag = flag | O_NONBLOCK;
  int ret = ::fcntl(sd, F_SETFL, nflag);

  flag = ::fcntl(sd, F_GETFD, 0);
  nflag |= FD_CLOEXEC;
  ret == -1? ret : (::fcntl(sd, F_SETFD, nflag));

  if (ret == -1) {
    //FIXME : logging
    return false;
  }
  return true;
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
  if (SetNoBlockingOrExec(ret) == false) {
    //FIXME : logging
    return ret;
  }
  return ret;
}

ssize_t ReadSocket(Socket_t sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t ReadvSocket(Socket_t sockfd, const struct iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t WriteSocket(Socket_t sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void CloseSocket(Socket_t sockfd) {
  if (::close(sockfd) < 0) {
    //FIXME : logging
  }
}

void ShutdownWrite(Socket_t sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    //FIXME : logging
  }
}
