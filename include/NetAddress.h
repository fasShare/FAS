#ifndef FAS_SOCKETNETADDRESS_H
#define FAS_SOCKETNETADDRESS_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>


class NetAddress {
public:
  NetAddress(ushort sa_family, int sa_port, const char *sa_ip);
  NetAddress(ushort sa_family, int sa_port, uint sa_ip);
  NetAddress(int sa_port, const char *sa_ip);
  NetAddress(int sa_port);
  struct sockaddr_in& addr();
  socklen_t addrLen();

private:
  struct sockaddr_in addr_;
};

#endif // FAS_SOCKETNETADDRESS_H
