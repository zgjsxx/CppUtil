#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>
#include "catch.hpp"
#include "common/include/Hash.h"

using namespace CppUtil;
using namespace CppUtil::Common;

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