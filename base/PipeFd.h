#ifndef FAS_PIPEFD_H
#define FAS_PIPEFD_H
#include <unistd.h>

namespace fas {

struct PipeFd {
    int End[2];
    int getWriteEnd() const {
        return End[1];
    }
    int getReadEnd() const {
        return End[0];
    }
    void closeWriteEnd() const {
        ::close(End[1]);
    }
    void closeReadEnd() const {
        ::close(End[0]);
    }
    ~PipeFd() {
        ::close(End[1]);
        ::close(End[0]);
    }
};

}
#endif
