#include <endian.h>
#include <typeinfo>
#include <iostream>

#include <NetAddress.h>


#include <boost/implicit_cast.hpp>

NetAddress::NetAddress(ushort sa_family,
                       int sa_port,
                       const char *sa_ip):
  family_(sa_family) {
  // FIXME : only AF_INET, no AF_INET6
  addr_.sin_family = sa_family;
  addr_.sin_port = htobe16(sa_port);
  addr_.sin_addr.s_addr = ::inet_addr(sa_ip);
}

struct sockaddr *NetAddress::addrPtr() {
  if (family_ == AF_INET) {
    return static_cast<struct sockaddr *>(boost::implicit_cast<void*>(&addr_));
  } else if (family_ == AF_INET6) {
    return static_cast<struct sockaddr *>(boost::implicit_cast<void*>(&addr6_));
  }
  return static_cast<struct sockaddr *>(boost::implicit_cast<void*>(&addr6_));
}

const struct sockaddr *NetAddress::addrPtr() const {
  if (family_ == AF_INET) {
    return static_cast<const struct sockaddr *>(boost::implicit_cast<const void*>(&addr_));
  } else if (family_ == AF_INET6) {
    return static_cast<const struct sockaddr *>(boost::implicit_cast<const void*>(&addr6_));
  }
  return static_cast<const struct sockaddr *>(boost::implicit_cast<const void*>(&addr6_));
}

socklen_t NetAddress::addrLen() const{
  if (family_ == AF_INET) {
    return sizeof(addr_);
  } else if (family_ == AF_INET6) {
    return sizeof(addr6_);
  }
  return sizeof(addr6_);
}

