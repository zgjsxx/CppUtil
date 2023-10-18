#pragma once
#include <string>
namespace CppUtil {
namespace Common {
void strToUpper(std::string& src);
void strToUpper(const std::string& src, std::string& dest);

void strToLower(std::string& src);
void strToLower(const std::string& src, std::string& dest);
}  // namespace Common
}  // namespace CppUtil
