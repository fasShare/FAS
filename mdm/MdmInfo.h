#ifndef FAS_INFOLOADER_H
#define FAS_INFOLOADER_H
#include <json/json.h>
#include <string>
#include <map>
#include <set>

namespace fas {

namespace mdm {

class MdmInfo {
public:
    MdmInfo();
    int load(const std::string& filename);
    std::string getValue(const std::string& root, const std::string& key) const;

    std::string getServerIp() const;
    short getServerPort() const;
    int getThreadNum() const;
    int getHashVNodeNum() const;
public:
    std::map<std::string, std::string> mdmInfo_;
    std::map<std::string, std::string> HashAddrMgrInfo_;
    std::map<std::string, std::map<std::string, std::string> *> infos_;
};

}

}

fas::mdm::MdmInfo* GET_MDM_INFO();

#endif // FAS_INFOLOADER_H
