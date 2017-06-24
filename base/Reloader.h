#ifndef FAS_RELOADER_H
#define FAS_RELOADER_H
#include <string>

namespace fas {

class Reloader {
public:
    virtual int need_reload() const = 0;
    virtual int reload() = 0;
    virtual int load() = 0;
    virtual std::string get_fname() const = 0;
};

}

#endif // FAS_RELOADER_H

