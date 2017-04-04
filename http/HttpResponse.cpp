#include <HttpResponse.h>

std::string fas::http::HttpResponse::getVersion() const {
  return this->version_;
}

void fas::http::HttpResponse::setVersion(const std::string& version) {
  this->version_ = version;
}

std::string fas::http::HttpResponse::getStateCode() const {
  return this->stateCode_;
}

void fas::http::HttpResponse::setStateCode(const std::string& stateCode) {
  this->stateCode_ = stateCode;
}

std::string fas::http::HttpResponse::getReason() const {
  return this->reason_;
}

void fas::http::HttpResponse::setReason(const std::string& reason) {
  this->reason_ = reason;
}

void fas::http::HttpResponse::updateHeadItem(const std::string& field, const std::string& value) {
  this->headers_[field] = value;
}
