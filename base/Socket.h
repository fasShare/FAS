#ifndef FAS_SOCKET_H
#define FAS_SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>



#include <Types.h>

namespace fas {

class NetAddress;

/*!
 * \brief The Socket class
 * Encapsulation of socket, listen, bind, fcntl(O_NONBLOCK, FD_CLOEXEC), accept, close.
 */
class Socket {
public:
  enum STATE {
    CLOSED,
    OPENED,
  };

  Socket(int domain, int type, int protocol);
  Socket(Socket_t sd);
  ~Socket();

  Socket_t getSocket() const;

  bool setNoBlocking();
  bool setExecClose();

  bool bind(const NetAddress& addr);
  bool listen(int backlog);
  bool connect(const NetAddress& addr);
  Socket_t accept(NetAddress& addr, bool noblockingexec);
  void close();
private:
  Socket_t socket_;
  uchar state_;
};

bool SetNoBlockingOrExec(Socket_t sd);

}
#endif // FAS_SOCKET_H
