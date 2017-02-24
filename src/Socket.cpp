#include <Socket.h>
#include <unistd.h>
#include <Log.h>
#include <string.h>
#include <errno.h>


Socket_t Socket(int domain, int type, int protocol) {
  Socket_t sd = ::socket(domain, type, protocol);
  if (sd == -1) {
    LOG_SYSERR(strerror(errno));
    return -1;
  }
  return sd;
}

Socket_t SocketNoBlockingOrExec(int domain, int type, int protocol) {
  Socket_t socket = Socket(domain, type, protocol);
  if(socket != -1) {
    if (false == SetNoBlockingOrExec(socket) ){
      socket = -1;
    }
  }
  return socket;
}


bool SetNoBlockingOrExec(Socket_t sd) {
  int flag = ::fcntl(sd, F_GETFL);
  int nflag = flag | O_NONBLOCK;
  int ret = ::fcntl(sd, F_SETFL, nflag);

  flag = ::fcntl(sd, F_GETFD, 0);
  nflag |= FD_CLOEXEC;
  ret == -1? ret : (::fcntl(sd, F_SETFD, nflag));
  if (ret == -1) {
    LOG_SYSERR((string("ERROR fcntl :") + strerror(errno).c_str()));
    return false;
  }
  return true;
}

bool SocketBind(Socket_t socket, NetAddress& addr) {
  int ret = ::bind(socket, (const struct sockaddr *)&addr.addr(), addr.addrLen());
  if (ret == -1) {
    LOG_ERROR((string("ERROR bind :") + strerror(errno).c_str()));
    return false;
  }
  return true;
}

bool SocketListen(Socket_t socket, int backlog) {
  int ret = ::listen(socket, backlog);
  if (ret == -1) {
    LOG_SYSERR(strerror(errno));
    return false;
  }
  return true;
}

Socket_t SocketAccept(Socket_t socket, struct sockaddr* addr, socklen_t* addrlen) {
  int ret = ::accept(socket, addr, addrlen);
  if(ret == -1) {
    LOG_ERROR((string("ERROR accept :") + strerror(errno).c_str()));
    return ret;
  }
  if (SetNoBlockingOrExec(ret) == false) {
    LOG_ERROR((string("ERROR SetNoBlockingOrExec :") + strerror(errno).c_str()));
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
    LOG_ERROR(strerror(errno).c_str());
  }
}

void ShutdownWrite(Socket_t sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    LOG_ERROR(strerror(errno).c_str());
  }
}
