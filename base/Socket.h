#ifndef FAS_SOCKET_H
#define FAS_SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

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
    Socket(int sd = -1);
    ~Socket();

    const int operator=(int sd);

    int getSocket() const;

    bool setNoBlocking();
    bool setExecClose();

    bool bind(const NetAddress& addr);
    bool listen(int backlog);
    bool connect(const NetAddress& addr);
    int accept(NetAddress& addr, bool noblockingexec);
private:
    int socket_;
    u_int8_t state_;
};

bool SetNoBlockingOrExec(int sd);

}
#endif // FAS_SOCKET_H
