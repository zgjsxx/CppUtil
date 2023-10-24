#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>
#include "catch.hpp"
#include "common/include/Hash.h"

using namespace CppUtil;
using namespace CppUtil::Common;

TEST_CASE("1", "Hash") {
  std::string str = "0123456789";
  std::string res = sha1sum(str);
  std::cout << res << std::endl;
  REQUIRE(res == "87acec17cd9dcd20a716cc2cf67417b71c8a7016");
}

TEST_CASE("2", "Hash") {
  std::string str = "abcdefg";
  std::string res = sha1sum(str);
  std::cout << res << std::endl;
  REQUIRE(res == "2fb5e13419fc89246865e7a324f476ec624e8740");
}