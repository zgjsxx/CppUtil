#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>
#include "catch.hpp"
#include "common/include/StringUtil.h"

using namespace CppUtil;
using namespace CppUtil::Common;

TEST_CASE("strToUpper-1", "StringUtil") {
  std::string str = "abcdefg";
  std::string res;
  strToUpper(str, res);
  std::cout << "res = " << res << std::endl;
  REQUIRE(res == "ABCDEFG");
}

TEST_CASE("strToUpper-2", "StringUtil") {
  std::string str = "11a22bc33def44g";
  std::string res;
  strToUpper(str, res);
  std::cout << res << std::endl;
  REQUIRE(res == "11A22BC33DEF44G");
}

TEST_CASE("strToUpper-3", "StringUtil") {
  std::string str = "11a22bc33def44g555sdadaodjakjkldadasjdsakd";
  std::string res;
  strToUpper(str, res);
  std::cout << res << std::endl;
  REQUIRE(res == "11A22BC33DEF44G555SDADAODJAKJKLDADASJDSAKD");
}

TEST_CASE("strToLower-1", "StringUtil") {
  std::string str = "ABCDEFG";
  std::string res;
  strToLower(str, res);
  std::cout << "res = " << res << std::endl;
  REQUIRE(res == "abcdefg");
}

TEST_CASE("strToLower-2", "StringUtil") {
  std::string str = "11A22BC33DEF44G";
  std::string res;
  strToLower(str, res);
  std::cout << res << std::endl;
  REQUIRE(res == "11a22bc33def44g");
}

TEST_CASE("strToLower-3", "StringUtil") {
  std::string str = "11A22BC33DEF44G555SDADAODJAKJKLDADASJDSAKD";
  std::string res;
  strToLower(str, res);
  std::cout << res << std::endl;
  REQUIRE(res == "11a22bc33def44g555sdadaodjakjkldadasjdsakd");
}