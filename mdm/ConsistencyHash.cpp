#include "ConsistencyHash.h"
#include <iostream>

fas::mdm::ConsistencyHash::ConsistencyHash(int virtualNodeNum) :
    virtualNodeNum_(virtualNodeNum),
    md5_(),
    nodes_(),
    hashMap_() {

}

bool fas::mdm::ConsistencyHash::addHashNodesFromString(std::string nodeName) {

    auto iter = nodes_.find(nodeName);
    if (iter != nodes_.end()) {
        return false;
    }

    std::shared_ptr<HashNode> node = std::make_shared<HashNode>(&md5_, nodeName, virtualNodeNum_);
    nodes_[nodeName] = node;

    std::vector<HashNode::KeyType> nodeKeys = node->getNodeKeys();

    for (auto iter = nodeKeys.begin(); iter != nodeKeys.end(); ++iter) {
        hashMap_.insert({
            *iter, node
        });
    }

    return true;
}

bool fas::mdm::ConsistencyHash::deleteHashNodesFromString(std::string nodeName) {
    auto iter = nodes_.find(nodeName);
    if (iter == nodes_.end()) {
        return true;
    }
    std::vector<HashNode::KeyType> nodeKeys = iter->second->getNodeKeys();

    for (auto iter = nodeKeys.begin(); iter != nodeKeys.end(); ++iter) {
        hashMap_.erase(*iter);
    }

    return true;
}

std::pair<fas::mdm::ConsistencyHash::MappingNodeIter, bool>
fas::mdm::ConsistencyHash::getMappingNodeFromKey(HashNode::KeyType key) {
    auto iter = hashMap_.upper_bound(key);
    if (iter == hashMap_.end()) {
        iter = hashMap_.begin();
    }
    if (hashMap_.end() != iter) {
        iter->second->increaseMappedCount(); //增加访问节点计数
    }

    if(hashMap_.size() == 0) {
      return std::pair<ConsistencyHash::MappingNodeIter, bool>(iter, false);
    }

    return std::pair<ConsistencyHash::MappingNodeIter, bool>(iter, true);
}

std::shared_ptr<fas::mdm::HashNode> fas::mdm::ConsistencyHash::getMappingNodeFromKeyString(std::string key) {
  std::shared_ptr<HashNode> retNode = nullptr;
  md5_.GenerateMD5(key.c_str(), key.size());
  std::pair<ConsistencyHash::MappingNodeIter, bool> mapNodeRet;

  mapNodeRet = getMappingNodeFromKey(md5_.ToULong32());

  if (mapNodeRet.second) {
    retNode = mapNodeRet.first->second;
  }

  return retNode;
}

void fas::mdm::ConsistencyHash::ShowNodeMsg() {
    for (auto iter = hashMap_.begin(); iter != hashMap_.end(); ++iter) {
        std::cout << iter->second->getInitName() << std::endl;
    }
}

void fas::mdm::ConsistencyHash::ShowMappingStatisticMsg() {
    for (auto iter = nodes_.begin(); iter != nodes_.end(); ++iter) {
        std::cout << iter->second->getInitName() << "  " << iter->second->getMappedCount() << endl;
    }
}
