#include <endian.h>
#include <typeinfo>
#include <iostream>

#include <NetAddress.h>

NetAddress::NetAddress(ushort sa_family,
                       int sa_port,
                       const char *sa_ip) {
  addr_.sin_family = sa_family;
  addr_.sin_port = htobe16(sa_port);
  addr_.sin_addr.s_addr = ::inet_addr(sa_ip);
}

NetAddress::NetAddress(ushort sa_family,
                       int sa_port,
                       uint sa_ip) {
  addr_.sin_family = sa_family;
  addr_.sin_port = htobe16(sa_port);
  addr_.sin_addr.s_addr = htobe32(sa_ip);
}

NetAddress::NetAddress(int sa_port,
                       const char *sa_ip):
    NetAddress(AF_INET, sa_port, sa_ip) {
}

NetAddress::NetAddress(int sa_port):
    NetAddress(AF_INET, sa_port, INADDR_ANY){
}

struct sockaddr_in& NetAddress::addr() {
  return addr_;
}

socklen_t NetAddress::addrLen() {
  return sizeof(addr_);
}

