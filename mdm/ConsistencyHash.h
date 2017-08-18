#ifndef FAS_CONSISTENCYHASH_H
#define FAS_CONSISTENCYHASH_H
#include <Md5.h>
#include <map>
#include <memory>
#include <HashNode.h>
#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>

namespace fas {

namespace mdm {

class ConsistencyHash
{
public:
    typedef boost::shared_ptr<ConsistencyHash> HashSPtr;
    typedef boost::shared_ptr<HashNode> SerNodeSptr;
    typedef std::map<HashNode::KeyType, boost::shared_ptr<HashNode>> MappingNodeType;
    typedef MappingNodeType::const_iterator MappingNodeIter;
    ConsistencyHash(int virtualNodeNum);

    bool addHashNodesFromString(std::string nodeName);
    bool deleteHashNodesFromString(std::string nodeName);

    std::pair<MappingNodeIter, bool> getMappingNodeFromKey(HashNode::KeyType key);
    boost::shared_ptr<HashNode> getMappingNodeFromKeyString(std::string key);

    void ShowNodeMsg();
    void ShowMappingStatisticMsg();
private:
    int virtualNodeNum_;
    MD5 md5_;
    std::map<std::string, boost::shared_ptr<HashNode>> nodes_;
    std::map<HashNode::KeyType, boost::shared_ptr<HashNode>> hashMap_;
};

}

}

#endif // FAS_CONSISTENCYHASH_H
