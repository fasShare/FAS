#include <string>


#include <Log.h>
#include <utilstring.h>


#include <boost/xpressive/xpressive_dynamic.hpp>

using namespace boost::xpressive;

bool fas::utils::StringSplitBychar(const std::string& src,
                                   const char delimter,
                                   std::vector<std::string>& ret,
                                   size_t startpos) {
  if (startpos == std::string::npos) {
    return true;
  }
  size_t firstStartPos = src.find_first_not_of(delimter, startpos);
  size_t firstEndPos = src.find_first_of(delimter, firstStartPos);
  if (firstEndPos == std::string::npos) {
    ret.push_back(src.substr(firstStartPos, src.length() - firstStartPos));
    //LOGGER_DEBUG << "firstStartPos = " << firstStartPos << " firstEndPos = " << src.length() << fas::Log::CLRF;
  } else {
    ret.push_back(src.substr(firstStartPos, firstEndPos - firstStartPos));
    //LOGGER_DEBUG << "firstStartPos = " << firstStartPos << " firstEndPos = " << firstEndPos << fas::Log::CLRF;
  }

  return StringSplitBychar(src, delimter, ret, firstEndPos);
}

std::string fas::utils::StringTrimLeft(const std::string& src) {
  size_t startTrimPos = src.find_first_not_of(' ');
  return src.substr(startTrimPos, src.length() - startTrimPos);
}

std::string fas::utils::StringTrimRight(const std::string& src) {
  return src.substr(0, src.find_last_not_of(' ') + 1);
}

std::string fas::utils::StringTrim(const std::string& src) {
  size_t startTrimPos = src.find_first_not_of(' ');
  size_t endTrimPos = src.find_last_not_of(' ') + 1;
  return src.substr(startTrimPos, endTrimPos - startTrimPos);
}

std::string fas::utils::StringRemoveRepeatChar(const std::string& src, const std::string& op) {
  //boost::xpressive::sregex replace = boost::xpressive::sregex::compile((std::string("(") + op + std::string("){2, }")).c_str());
  std::string reg_format = std::string("(") + op + std::string("){2,}");
  sregex replace = sregex::compile(reg_format.c_str());
  return regex_replace(src, replace, op.c_str());
}

std::string fas::utils::StringGetSuffix(const std::string& src) {
  size_t pos = src.find_last_of('.');
  if (pos == std::string::npos) {
    return "";
  }
  return src.substr(pos, src.length() - pos);
}
