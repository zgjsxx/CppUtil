#include <algorithm>
#include "common/include/StringUtil.h"

namespace CppUtil {
namespace Common {
void strToUpper(std::string& src) {
  std::transform(src.begin(), src.end(), src.begin(), ::toupper);
}

void strToUpper(const std::string& src, std::string& dest) {
  dest.reserve(src.size());
  std::transform(src.begin(), src.end(), dest.begin(), ::toupper);
}

void strToLower(std::string& src) {
  std::transform(src.begin(), src.end(), src.begin(), ::tolower);
}

void strToLower(const std::string& src, std::string& dest) {
  dest.reserve(src.size());
  std::transform(src.begin(), src.end(), dest.begin(), ::tolower);
}
}  // namespace Common
}  // namespace CppUtil