#ifndef FAS_UTILSTRING_H
#define FAS_UTILSTRING_H
#include <vector>
#include <string>

namespace fas {

namespace utils {

bool StringSplitBychar(const std::string& src, const char delimter,
                       std::vector<std::string>& ret, std::size_t startpos);

std::string StringTrimLeft(const std::string& src);
std::string StringTrimRight(const std::string& src);
std::string StringTrim(const std::string& src);
std::string StringRemoveRepeatChar(const std::string& src, const std::string& op);
std::string StringGetSuffix(const std::string& src);
}

}

#endif //FAS_UTILSTRING_H
