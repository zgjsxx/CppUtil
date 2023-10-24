#include "common/include/Hash.h"

namespace CppUtil {
namespace Common {

static const char hex_chars[] = "0123456789abcdef";

std::string sha1sum(std::string& str) {
  unsigned char output[SHA_DIGEST_LENGTH] = {0};
  SHA1((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA_DIGEST_LENGTH] = {0};
  int j = 0;
  unsigned int c;

  for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
    c = (output[i] >> 4) & 0x0f;
    output_hex[j++] = hex_chars[c];
    output_hex[j++] = hex_chars[output[i] & 0x0f];
  }

  std::string res;
  res.append((const char*)output_hex, 2 * SHA_DIGEST_LENGTH);
  return res;
}
}  // namespace Common
}  // namespace CppUtil