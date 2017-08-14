#ifndef FAS_POLLFACTORY_H
#define FAS_POLLFACTORY_H
#include <new>

#include <Poll.h>
#include <PollerFactory.h>

namespace fas {

class PollFactory : public PollerFactory {
public:
    Poll* getPoller() {
        return new (std::nothrow) Poll();
    }
};

}

#endif //FAS_EPOLLFACTORY_H
