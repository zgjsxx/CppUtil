#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <string.h>
#include <iostream>
#include "net/protocol/selfDefine/SimpleProtoParser.h"

TEST_CASE("1", "SimpleProtoParser") {
  SimpleProtoMsg simpleProtoMsg;
  simpleProtoMsg.head.len = 5;
  simpleProtoMsg.body = "hello";
  int len = sizeof(simpleProtoMsg.head);
  len += simpleProtoMsg.body.size();
  uint8_t* data = new uint8_t[len];
  SimpleProtoEncode(&simpleProtoMsg, data);

  SimpleProtoParser parser;
  parser.init();
  parser.parseMsg(data, 1);
  REQUIRE(parser.parseFinish() == false);
  parser.parseMsg(data + 1, 2);
  REQUIRE(parser.parseFinish() == false);
  parser.parseMsg(data + 3, len - 3);
  REQUIRE(parser.parseFinish() == true);
  REQUIRE(parser.getMsg().body == "hello");

  delete[] data;
}