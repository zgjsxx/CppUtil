#include "common/include/Hash.h"

namespace CppUtil {
namespace Common {

const int HEXCHAR_SIZE = 16;
static const char hex_chars[] = "0123456789abcdef";
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static char getIndex(char c) {
  for (int i = 0; i < HEXCHAR_SIZE; ++i) {
    if (c == hex_chars[i]) {
      return i;
    }
  }
  return -1;
}

void convertHexStringToBinary(const unsigned char* ori, int len,
                              unsigned char* des, int desLen) {
  // input  |1d|2a|   ==> output |00011101|00101010
  int j = 0;
  for (int i = 0; i < len; i++) {
    char c = getIndex(ori[i]);
    if (i % 2 == 0) {
      des[i / 2] |= (c << 4);
    } else {
      des[i / 2] |= c;
    }
  }
}

void convertBinaryToHexString(const unsigned char* ori, int len,
                              unsigned char* des, int desLen) {
  // input |00011101|00101010|   ==> output |1d|2a|
  unsigned int c;
  int j = 0;
  for (int i = 0; i < len; ++i) {
    c = (ori[i] >> 4) & 0x0f;
    des[j++] = hex_chars[c];
    des[j++] = hex_chars[ori[i] & 0x0f];
  }
}

std::string sha1sumHex(std::string& str) {
  unsigned char output[SHA_DIGEST_LENGTH] = {0};
  SHA1((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA_DIGEST_LENGTH] = {0};
  convertBinaryToHexString(output, SHA_DIGEST_LENGTH, output_hex,
                           2 * SHA_DIGEST_LENGTH);
  std::string res;
  res.append((const char*)output_hex, 2 * SHA_DIGEST_LENGTH);
  return res;
}

std::string sha1sum(std::string& str) {
  unsigned char output[SHA_DIGEST_LENGTH] = {0};
  SHA1((const unsigned char*)str.c_str(), str.size(), output);
  std::string res;
  res.append((const char*)output, SHA_DIGEST_LENGTH);
  return res;
}

// output is hex string
std::string sha224sumHex(std::string& str) {
  unsigned char output[SHA224_DIGEST_LENGTH] = {0};
  SHA224((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA224_DIGEST_LENGTH] = {0};
  convertBinaryToHexString(output, SHA224_DIGEST_LENGTH, output_hex,
                           2 * SHA224_DIGEST_LENGTH);
  std::string res;
  res.append((const char*)output_hex, 2 * SHA224_DIGEST_LENGTH);
  return res;
}

std::string sha224sum(std::string& str) {
  unsigned char output[SHA224_DIGEST_LENGTH] = {0};
  SHA224((const unsigned char*)str.c_str(), str.size(), output);
  std::string res;
  res.append((const char*)output, SHA224_DIGEST_LENGTH);
  return res;
}

std::string sha256sum(std::string& str) {
  unsigned char output[SHA256_DIGEST_LENGTH] = {0};
  SHA256((const unsigned char*)str.c_str(), str.size(), output);
  std::string res;
  res.append((const char*)output, SHA256_DIGEST_LENGTH);
  return res;
}

std::string sha256sumHex(std::string& str) {
  unsigned char output[SHA256_DIGEST_LENGTH] = {0};
  SHA256((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA256_DIGEST_LENGTH] = {0};
  convertBinaryToHexString(output, SHA256_DIGEST_LENGTH, output_hex,
                           2 * SHA256_DIGEST_LENGTH);

  std::string res;
  res.append((const char*)output_hex, 2 * SHA256_DIGEST_LENGTH);
  return res;
}

std::string sha384sum(std::string& str) {
  unsigned char output[SHA384_DIGEST_LENGTH] = {0};
  SHA384((const unsigned char*)str.c_str(), str.size(), output);
  std::string res;
  res.append((const char*)output, SHA384_DIGEST_LENGTH);
  return res;
}

std::string sha384sumHex(std::string& str) {
  unsigned char output[SHA384_DIGEST_LENGTH] = {0};
  SHA384((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA384_DIGEST_LENGTH] = {0};
  convertBinaryToHexString(output, SHA384_DIGEST_LENGTH, output_hex,
                           2 * SHA384_DIGEST_LENGTH);
  std::string res;
  res.append((const char*)output_hex, 2 * SHA384_DIGEST_LENGTH);
  return res;
}

std::string sha512sum(std::string& str) {
  unsigned char output[SHA512_DIGEST_LENGTH] = {0};
  SHA512((const unsigned char*)str.c_str(), str.size(), output);
  std::string res;
  res.append((const char*)output, SHA512_DIGEST_LENGTH);
  return res;
}

std::string sha512sumHex(std::string& str) {
  unsigned char output[SHA512_DIGEST_LENGTH] = {0};
  SHA512((const unsigned char*)str.c_str(), str.size(), output);
  unsigned char output_hex[2 * SHA512_DIGEST_LENGTH] = {0};

  convertBinaryToHexString(output, SHA512_DIGEST_LENGTH, output_hex,
                           2 * SHA512_DIGEST_LENGTH);
  std::string res;
  res.append((const char*)output_hex, 2 * SHA512_DIGEST_LENGTH);
  return res;
}

std::string base64_encode(unsigned char const* bytes_to_encode,
                          unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

    while ((i++ < 3)) ret += '=';
  }

  return ret;
}
std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

}  // namespace Common
}  // namespace CppUtil