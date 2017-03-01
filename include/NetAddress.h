#ifndef FAS_SOCKETNETADDRESS_H
#define FAS_SOCKETNETADDRESS_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

/*!
 * \brief The NetAddress class
 * Encapsulation of struct sockaddr_in
 */
class NetAddress {
public:
  NetAddress(ushort sa_family, int sa_port, const char *sa_ip);
  NetAddress() = default;
  struct sockaddr *addrPtr();
  const struct sockaddr *addrPtr() const;
  socklen_t addrLen() const;

private:
  sa_family_t family_;
  union {
  struct sockaddr_in addr_;
  struct sockaddr_in6 addr6_;
  };
};

#endif // FAS_SOCKETNETADDRESS_H
