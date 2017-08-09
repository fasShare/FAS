#include <json/json.h>
#include <fstream>

#include <MdmInfoLoader.h>
#include <MdmInfo.h>
#include <Log.h>

#include <boost/lexical_cast.hpp>

fas::mdm::MdmInfo::MdmInfo() :
    mdmInfo_(),
    HashAddrMgrInfo_(),
    infos_() {
    infos_["mdmInfo"] = &mdmInfo_;
    infos_["HashAddrMgr"] = &HashAddrMgrInfo_;
}


int fas::mdm::MdmInfo::load(const std::string& filename) {
    std::ifstream in;
    in.open(filename, std::ios_base::in);
    if (!in.is_open()) {
        LOGGER_ERROR(" Open info file error!");
        return -1;
    }

    Json::Reader jsonReader;
    Json::Value root;

    if (!jsonReader.parse(in, root)) {
        LOGGER_ERROR("Parse info file error!");
        return -1;
    }

    if (root.isMember("mdm")) {
        mdmInfo_["PollTimeOut"] = root["mdm"]["PollTimeOut"].asString();
        mdmInfo_["ServerIp"] = root["mdm"]["ServerIp"].asString();
        mdmInfo_["ServerPort"] = root["mdm"]["ServerPort"].asString();
        mdmInfo_["ThreadNum"] = root["mdm"]["ThreadNum"].asString();
        mdmInfo_["HashVNodeNum"] = root["mdm"]["HashVNodeNum"].asString();
    } else {
        LOGGER_ERROR("can not find mdminfo!");
        return -1;
    }

    if (root.isMember("HashAddrMgr")) {
        HashAddrMgrInfo_["master"] = root["HashAddrMgr"]["master"].asString();
        HashAddrMgrInfo_["slave"] = root["HashAddrMgr"]["slave"].asString();
    } else {
        LOGGER_ERROR("can not find HashAddrMgrInfo!");
        return -1;
    }

    return 0;
}

std::string fas::mdm::MdmInfo::getValue(const std::string& root, const std::string& key) const {
    std::string value = "";

    auto info = infos_.find(root);
    if (info != infos_.end()) {
        auto viter = info->second->find(key);
        if (viter != info->second->end()) {
            value = viter->second;
        }
    }

    return value;
}

std::string fas::mdm::MdmInfo::getServerIp() const {
    return getValue("mdm", "serverIp");
}

short fas::mdm::MdmInfo::getServerPort() const {
    std::string port = getValue("mdm", "ServerPort");
    if (port.empty()) {
        return -1;
    }
    return boost::lexical_cast<short>(port);
}

int fas::mdm::MdmInfo::getThreadNum() const {
    std::string num = getValue("mdm", "ThreadNum");
    if (num.empty()) {
        return -1;
    }
    return boost::lexical_cast<int>(num);
}

int fas::mdm::MdmInfo::getHashVNodeNum() const {
    std::string num = getValue("mdm", "HashVNodeNum");
    if (num.empty()) {
        return -1;
    }
    return boost::lexical_cast<int>(num);
}

