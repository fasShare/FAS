#ifndef FAS_FASINFOLOADER_H
#define FAS_FASINFOLOADER_H
#include <FasInfo.h>
#include <Reloader.h>

namespace fas {

class FasInfoLoader : public Reloader {
private:
    time_t _tm;
    unsigned int _using_no;
    std::string _fname;
    FasInfo* _p_content[ 2 ];

public:
    FasInfoLoader(const char* fname);
    virtual ~FasInfoLoader();
    int need_reload() const;
    int reload();
    int load();
    FasInfo* get_content();
    const FasInfo* get_content() const;
    std::string get_fname() const { return _fname; }
};

}

#endif //FAS_FASINFOLOADER_H
