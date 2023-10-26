#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>
#include "catch.hpp"
#include "common/include/Hash.h"

using namespace CppUtil;
using namespace CppUtil::Common;

TEST_CASE("convertHexStringToBinary-1", "Hash") {
  //|1d|2a|   ==> output |00011101|00101010
  std::string str;
  str.append("1d2a");
  unsigned char des[2];
  convertHexStringToBinary((const unsigned char*)str.c_str(), str.size(), des,
                           2);
  REQUIRE(des[0] == 0x1d);
  REQUIRE(des[1] == 0x2a);
}

TEST_CASE("convertHexStringToBinary-2", "Hash") {
  //|1d|20|   ==> output |00011101|00101010
  std::string str;
  str.append("1d2");
  unsigned char des[2];
  convertHexStringToBinary((const unsigned char*)str.c_str(), str.size(), des,
                           2);
  REQUIRE(des[0] == 0x1d);
  REQUIRE(des[1] == 0x20);
}

TEST_CASE("sha1sum-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha1sum(str);

  std::string resHex = sha1sumHex(str);
  std::string golden;
  golden.resize(resHex.size() / 2 + 1, 0);
  convertHexStringToBinary((const unsigned char*)resHex.c_str(), resHex.size(),
                           (unsigned char*)golden.c_str(), golden.size());
  REQUIRE(res.compare(golden));
}

TEST_CASE("sha1sumHex-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha1sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res == "87acec17cd9dcd20a716cc2cf67417b71c8a7016");
}

TEST_CASE("sha1sumHex-2", "Hash") {
  std::string str = "abcdefg";
  std::string res = sha1sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res == "2fb5e13419fc89246865e7a324f476ec624e8740");
}

TEST_CASE("sha224sumHex-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha224sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res == "f28ad8ecd48ba6f914c114821685ad08f0d6103649ff156599a90426");
}

TEST_CASE("sha256sumHex-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha256sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res ==
          "84d89877f0d4041efb6bf91a16f0248f2fd573e6af05c19f96bedb9f882f7882");
}

TEST_CASE("sha256sumHex-2", "Hash") {
  std::string str = "helloworld";
  std::string res = sha256sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res ==
          "936a185caaa266bb9cbe981e9e05cb78cd732b0b3280eb944412bb6f8f8f07af");
}

TEST_CASE("sha384sumHex-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha384sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res ==
          "90ae531f24e48697904a4d0286f354c50a350ebb6c2b9efcb22f71c96ceaeffc11c6"
          "095e9ca0df0ec30bf685dcf2e5e5");
}

TEST_CASE("sha512sumHex-1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha512sumHex(str);
  std::cout << res << std::endl;
  REQUIRE(res ==
          "bb96c2fc40d2d54617d6f276febe571f623a8dadf0b734855299b0e107fda32cf6b6"
          "9f2da32b36445d73690b93cbd0f7bfc20e0f7f28553d2a4428f23b716e90");
}

// right value is get from online website
// https://base64.guru/converter/encode/hex
TEST_CASE("base64_encode-1", "Hash") {
  std::string str = "abcdefg";
  std::string res =
      base64_encode((const unsigned char*)str.c_str(), str.size());
  std::cout << res << std::endl;
  REQUIRE(res == "YWJjZGVmZw==");
}

TEST_CASE("base64_encode-2", "Hash") {
  std::string str = "abcdefg21313";
  std::string res =
      base64_encode((const unsigned char*)str.c_str(), str.size());
  std::cout << res << std::endl;
  REQUIRE(res == "YWJjZGVmZzIxMzEz");
}

TEST_CASE("base64_decode-1", "Hash") {
  std::string str = "YWJjZGVmZw==";
  std::string res = base64_decode(str);
  std::cout << res << std::endl;
  REQUIRE(res == "abcdefg");
}

TEST_CASE("base64_decode-2", "Hash") {
  std::string str = "YWJjZGVmZzIxMzEz";
  std::string res = base64_decode(str);
  std::cout << res << std::endl;
  REQUIRE(res == "abcdefg21313");
}

TEST_CASE("websocket-handshake", "Hash") {
  std::string key = "sN9cRrP/n9NdMgdcy2VJFQ==";
  key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string res = sha1sum(key);
  res = base64_encode((const unsigned char*)res.c_str(), res.size());
  std::cout << res << std::endl;

  REQUIRE(res == "fFBooB7FAkLlXgRSz0BT3v4hq5s=");
}