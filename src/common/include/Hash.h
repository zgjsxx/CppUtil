#pragma once

#include <string>
#include "openssl/sha.h"

namespace CppUtil {
namespace Common {

void convertHexStringToBinary(const unsigned char* ori, int len,
                              unsigned char* des, int desLen);

void convertBinaryToHexString(const unsigned char* ori, int len,
                              unsigned char* des, int desLen);

// == sha series function ==
// output is hex string
std::string sha1sumHex(std::string& str);

std::string sha1sum(std::string& str);

// output is hex string
std::string sha224sumHex(std::string& str);

std::string sha224sum(std::string& str);

// output is hex string
std::string sha256sumHex(std::string& str);

std::string sha256sum(std::string& str);

// output is hex string
std::string sha384sumHex(std::string& str);

std::string sha384sum(std::string& str);

// output is hex string
std::string sha512sumHex(std::string& str);

std::string sha512sum(std::string& str);

//=== base64 ==
std::string base64_encode(unsigned char const* bytes_to_encode,
                          unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

}  // namespace Common
}  // namespace CppUtil