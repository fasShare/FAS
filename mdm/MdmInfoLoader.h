#ifndef FAS_FASINFOLOADER_H
#define FAS_FASINFOLOADER_H
#include <MdmInfo.h>
#include <Reloader.h>

namespace fas {

namespace mdm {

class MdmInfoLoader : public Reloader {
private:
    time_t _tm;
    unsigned int _using_no;
    std::string _fname;
    MdmInfo* _p_content[ 2 ];

public:
    MdmInfoLoader(const char* fname);
    virtual ~MdmInfoLoader();
    int need_reload() const;
    int reload();
    int load();
    MdmInfo* get_content();
    const MdmInfo* get_content() const;
    std::string get_fname() const { return _fname; }
};

}

}

#endif //FAS_FASINFOLOADER_H
