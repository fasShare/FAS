#ifndef FAS_EPOLLFACTORY_H
#define FAS_EPOLLFACTORY_H
#include <new>

#include <Epoll.h>
#include <PollerFactory.h>

namespace fas {

class EpollFactory : public PollerFactory {
public:
    Epoll* getPoller() {
        return new (std::nothrow) Epoll();
    }
};

}

#endif //FAS_EPOLLFACTORY_H
