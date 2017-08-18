#ifndef FAS_MEMCACHEDPROTOCOL_H
#define FAS_MEMCACHEDPROTOCOL_H
#include <string>
#include <set>
#include <vector>

namespace fas {

namespace mdm {

class MemProtocol {
public:
    MemProtocol();
    bool inCmdSet(const std::string& cmd) const;
    bool isStorageCommand(const std::string& cmd) const;
    bool isRetrievalCommand(const std::string& cmd) const;
    bool isOtherCommand(const std::string& cmd) const;
    bool isDeleteCommand(const std::string& cmd) const;
    bool isIncrDecrCommand(const std::string& cmd) const;
    const std::string& getNoreply() const;
private:
    std::set<std::string> stroage_cmd_;
    std::vector<std::string> stroage_cmd_res_;

    std::set<std::string> retriveal_cmd_;
    std::vector<std::string> retriveal_cmd_res_;

    std::set<std::string> other_cmd_;
    std::vector<std::string> other_cmd_res_;

    std::set<std::string> delete_cmd_;
    std::vector<std::string> delete_cmd_res_;

    std::set<std::string> incr_decr_cmd_;
    std::vector<std::string> incr_decr_cmd_res_;

    const static std::string noreply_;
};

}

}

#endif // FAS_MEMCACHEDPROTOCOL_H

