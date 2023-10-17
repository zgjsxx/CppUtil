#define CATCH_CONFIG_MAIN

#include "net/serverBase/include/Buffer.h"

#include "catch.hpp"
using namespace CppUtil;
using namespace CppUtil::Net;

TEST_CASE("1", "Buffer") {
  Buffer buffer;
  buffer.append("string");
  REQUIRE(buffer.readableBytes() == 6);
  std::string str;
  str.append(buffer.peek(), buffer.readableBytes());
  REQUIRE(str == "string");
  buffer.retrieveAll();
  REQUIRE(buffer.readableBytes() == 0);
}

TEST_CASE("2", "Buffer") {
  Buffer buffer;
  buffer.append("string");
  REQUIRE(buffer.readableBytes() == 6);
  std::string str;
  str.append(buffer.peek(), buffer.readableBytes());
  REQUIRE(str == "string");
  buffer.retrieveAll();
  REQUIRE(buffer.readableBytes() == 0);
  buffer.append("demo");
  REQUIRE(buffer.readableBytes() == 4);
  str.clear();
  str.append(buffer.peek(), buffer.readableBytes());
  REQUIRE(str == "demo");
  buffer.retrieveAll();
  REQUIRE(buffer.readableBytes() == 0);
}