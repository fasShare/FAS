#ifndef FAS_HTTPCOMMON_H
#define FAS_HTTPCOMMON_H
#include <string>
#include <map>

namespace fas {

namespace http {

class Method {
public:
  const static std::string GET;
  const static std::string PUT;
  const static std::string HEAD;
  const static std::string TRACE;
  const static std::string POST;
  const static std::string DELETE;
  const static std::string OPTIONS;
};

class ServerOptions {
public:
  ServerOptions();
  ~ServerOptions();
  std::string getServerPath() const;
  void setServerPath(const std::string& path);
private:
  std::string serverPath_;
};

class ContentTypes {
public:
  ContentTypes();
  std::string getType(const std::string& suffix);
private:
  std::map<std::string, std::string> suffixTypes_;
};

}

}

#endif // FAS_HTTPCOMMON_H

