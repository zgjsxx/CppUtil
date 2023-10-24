#define CATCH_CONFIG_MAIN

#include <cstdio>
#include <string>
#include "catch.hpp"
#include "net/serverBase/include/Buffer.h"

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

TEST_CASE("3", "Buffer") {
  int tmp = 0x12345678;
  Buffer buffer;
  buffer.appendInt32(tmp);
  int tmp2 = buffer.readInt32();
  REQUIRE(tmp2 == 0x12345678);

  tmp = 0x01;
  buffer.appendLeInt32(tmp);
  tmp2 = buffer.readInt32();
  REQUIRE(tmp2 == 0x01000000);

  tmp = 0x01;
  buffer.appendLeInt32(tmp);
  tmp2 = buffer.readLeInt32();
  REQUIRE(tmp2 == 0x01);
}