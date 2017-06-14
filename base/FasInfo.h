#ifndef FAS_INFOLOADER_H
#define FAS_INFOLOADER_H
#include <json/json.h>
#include <string>
#include <map>
#include <set>

class FasInfo
{
public:
  FasInfo();
  int load(const std::string& filename);
  std::string getValue(const std::string& root, const std::string& key) const;
  std::string getPollerType() const;
  int getPollerTimeout() const;
  int getPollerNum() const;
  int getThreadNum() const;
  short getServerPort() const;
  std::string getServerIp() const;
public:
  std::map<std::string, std::string> fasInfo_;
  std::map<std::string, std::string> pollerInfo_;
  std::map<std::string, std::map<std::string, std::string> *> infos_;
};

#endif // FAS_INFOLOADER_H
