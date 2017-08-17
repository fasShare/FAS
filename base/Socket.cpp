#include <unistd.h>
#include <string.h>
#include <errno.h>


#include <Socket.h>
#include <Thread.h>
#include <Log.h>
#include <NetAddress.h>

fas::Socket::Socket(int domain, int type, int protocol) :
    socket_(::socket(domain, type, protocol)),
    state_(Socket::STATE::OPENED) {
}

fas::Socket::Socket(int sd) :
    socket_(sd),
    state_(Socket::STATE::CLOSED) {
}

const int fas::Socket::operator=(int sd) {
    socket_ = sd;
    return socket_;
}

int fas::Socket::getSocket() const {
    return socket_;
}

// FIXME : invoking other func
bool fas::Socket::setNoBlocking() {
    int flag = ::fcntl(socket_, F_GETFL);
    flag |= O_NONBLOCK;
    int ret = ::fcntl(socket_, F_SETFL, flag);
    if (ret == -1) {
        // FIXME : Log msg
        return false;
    }
    return true;
}

// FIXME : invoking other func
bool fas::Socket::setExecClose() {
    int flag = ::fcntl(socket_, F_GETFD, 0);
    flag |= FD_CLOEXEC;
    int ret = ::fcntl(socket_, F_SETFD, flag);
    if (ret == -1) {
        // FIXME : Log msg
        return false;
    }
    return true;
}

bool fas::Socket::bind(const NetAddress& addr) {
    int ret = ::bind(socket_, addr.addrPtr(), addr.addrLen());
    if (ret == -1) {
        LOGGER_SYSERR("bind error : " << ::strerror(errno));
        return false;
    }
    return true;
}

bool fas::Socket::listen(int backlog) {
    int ret = ::listen(socket_, backlog);
    if (ret == -1) {
        LOGGER_SYSERR("listen error : " << ::strerror(errno));
        return false;
    }
    return true;
}

bool fas::Socket::connect(const NetAddress& addr) {
    int ret = ::connect(socket_, addr.addrPtr(), addr.addrLen());
    if (ret == -1) {
        if (errno == EINPROGRESS) {
            return true;
        }
        LOGGER_SYSERR("connect error : " << ::strerror(errno));
        return false;
    }
    return true;
}

int fas::Socket::accept(fas::NetAddress& addr, bool noblockingexec) {
    socklen_t len = addr.addrLen();
    int ret = ::accept(socket_, addr.addrPtr(), &len);
    if(ret == -1) {
        LOGGER_SYSERR("accept error : " << ::strerror(errno));
        return ret;
    }
    if (!noblockingexec) {
        return ret;
    }
    if (SetNoBlockingOrExec(ret) == false) {
        return ret;
    }
    return ret;
}

fas::Socket::~Socket() {
    LOGGER_TRACE("tid : " << gettid() <<  " socket close!");
    ::close(socket_);
    state_ = Socket::STATE::CLOSED;
}

bool fas::SetNoBlockingOrExec(int sd) {
    int flag = ::fcntl(sd, F_GETFL);
    int nflag = flag | O_NONBLOCK;
    int ret = ::fcntl(sd, F_SETFL, nflag);

    flag = ::fcntl(sd, F_GETFD, 0);
    nflag |= FD_CLOEXEC;
    ret == -1? ret : (::fcntl(sd, F_SETFD, nflag));
    if (ret == -1) {
        LOGGER_SYSERR("accept error : " << ::strerror(errno));
        return false;
    }
    return true;
}


