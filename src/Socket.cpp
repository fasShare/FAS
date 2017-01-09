#include "Socket.h"
#include <unistd.h>
#include <iostream>
#include <string.h>

using namespace std;

Socket::Socket(int socket):
    sd(socket) {
	bool ret = Socket_set_noblock();
	if (ret) {
		available = true;
	} else {
		available = false;
	}
}

Socket::Socket(int domain, int type, int protocol) {
  //FIXME : IF ANY ERROR
  sd = socket(domain, type, protocol);
	bool ret = Socket_set_noblock();
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
		Socket_set_noblock();
	}

	return *this;
}


bool Socket::Socket_set_noblock() {
    cout << "Socket_set_noblock sd = " << sd << endl;
  int flag = fcntl(sd, F_GETFL);
  int nflag = flag | O_NONBLOCK;
  if (-1 == fcntl(sd, F_SETFL, nflag) ){
    cout << "Socket_set_noblock error : "<< strerror(errno) << endl;
    return false;
  }
  return true;
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


int Socket::Socket_accept(struct sockaddr* addr, socklen_t* addrlen) {
	int ret = accept(sd, addr, addrlen);
	if (ret == -1) {
		//FIXME:check error
		return ret;
	}
	return ret;
}


int Socket::Socket_get_sd() {
  return sd;
}

void Socket::Socket_close() {
  close(sd);
}

