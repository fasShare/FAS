#ifndef FAS_HASHNODE_H
#define FAS_HASHNODE_H

#include <vector>
#include <string>
#include <Md5.h>

namespace fas {

namespace mdm {

class HashNode
{
public:
    typedef unsigned long KeyType;
    HashNode(MD5 *md5, std::string initName, int nameNum);
    const std::vector<KeyType>& getNodeKeys() const;
    std::string getInitName() const;
    void increaseMappedCount();
    unsigned long long getMappedCount() const;

    std::string getIp() const;
    short getPort() const;


private:
    std::vector<KeyType> keys_;
    std::string initName_;
    int nameNum_;
    MD5 *md5_;

    unsigned long long  mappedCount_;

    std::string ip_;
    short port_;
};

}

}

#endif // FAS_HASHNODE_H
