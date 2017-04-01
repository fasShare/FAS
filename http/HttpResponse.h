#ifndef FAS_HTTPREPONSE_H
#define FAS_HTTPREPONSE_H
#include <string>
#include <map>

namespace fas {

namespace http {

class HttpResponse {
public:
  std::string getVersion() const;
  void setVersion(const std::string& version);
  std::string getStateCode() const;
  void setStateCode(const std::string& stateCode);
  std::string getReason() const;
  void setReason(const std::string& reason);

  void updateHeadItem(const std::string& field, const std::string& value);
private:
  std::string version_;
  std::string stateCode_;
  std::string reason_;

  std::map<std::string, std::string> headers_;
};

}

}

#endif // FAS_HTTPREPONSE_H

