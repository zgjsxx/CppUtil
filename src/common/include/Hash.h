#pragma once

#include <string>
#include "openssl/sha.h"

namespace CppUtil {
namespace Common {

std::string sha1sum(std::string& str);

}  // namespace Common
}  // namespace CppUtil