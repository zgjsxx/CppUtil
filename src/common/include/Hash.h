#pragma once

#include <string>
#include "openssl/sha.h"

namespace CppUtil {
namespace Common {

std::string base64_encode(unsigned char const* bytes_to_encode,
                          unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

// output is hex string
std::string sha1sumHex(std::string& str);

std::string sha1sum(std::string& str);
}  // namespace Common
}  // namespace CppUtil