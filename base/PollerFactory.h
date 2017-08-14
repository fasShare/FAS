#ifndef FAS_POLLERFACTORY_H
#define FAS_POLLERFACTORY_H
#include <Poller.h>
#include <VirtualBase.h>

namespace fas{

class PollerFactory : public VirtualBase {
public:
    virtual Poller *getPoller() = 0;
};

}

#endif //FAS_POLLERFACTORY_H
