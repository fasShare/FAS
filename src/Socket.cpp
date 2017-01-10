#include "Socket.h"
#include <unistd.h>
#include <iostream>
#include <string.h>

using namespace std;

Socket::Socket(int socket):
    sd(socket) {
    bool ret = set_noblock();
	if (ret) {
		available = true;
	} else {
		available = false;
	}
}

Socket::Socket(int domain, int type, int protocol) {
  //FIXME : IF ANY ERROR
  sd = socket(domain, type, protocol);
    bool ret = set_noblock();
	if (ret) {
		available = true;
	} else {
		available = false;
	}
}

Socket::Socket(int domain, int type):
  Socket(domain, type, 0){
}

Socket& Socket::operator= (const int sd) {
    cout << "Socket::operator= (const int sd)" << endl;
	if (sd == -1) {
		available = false;
	} else {
        this->sd = sd;
        set_noblock();
	}

	return *this;
}


bool Socket::set_noblock() {
    cout << "set_noblock sd = " << sd << endl;
  int flag = fcntl(sd, F_GETFL);
  int nflag = flag | O_NONBLOCK;
  if (-1 == fcntl(sd, F_SETFL, nflag) ){
    cout << "set_noblock error : "<< strerror(errno) << endl;
    return false;
  }
  return true;
}


bool Socket::bind(NetAddress &addr) {
  int ret = ::bind(sd, (const struct sockaddr *)&addr.get_addr(), \
                 addr.get_addr_len());
  return ret == -1 ? false : true;
}

bool Socket::listen(int backlog) {
  int ret = ::listen(sd, backlog);
  return ret == -1 ? false : true;
}

bool Socket::bind(const struct sockaddr *addr,
                        socklen_t addrlen) {
  int ret = ::bind(sd, addr, addrlen);
  return ret == -1 ? false : true;
}


int Socket::accept(struct sockaddr* addr, socklen_t* addrlen) {
    int ret = ::accept(sd, addr, addrlen);
	if (ret == -1) {
		//FIXME:check error
		return ret;
	}
	return ret;
}


int Socket::get_sd() {
  return sd;
}

void Socket::close() {
  ::close(sd);
}

