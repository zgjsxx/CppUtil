#include <string.h>
#include <iostream>
#include "net/protocol/selfDefine/SimpleProtoParser.h"

int main() {
  SimpleProtoMsg simpleProtoMsg;
  simpleProtoMsg.head.version = 0x01;
  simpleProtoMsg.head.magic = 0x02;
  simpleProtoMsg.head.len = 5;
  simpleProtoMsg.body.append("hello", 5);
  int len = sizeof(simpleProtoMsg.head);
  len += simpleProtoMsg.body.size();
  uint8_t* data = new uint8_t[len];
  memcpy(data, &simpleProtoMsg.head, sizeof(simpleProtoMsg.head));
  memcpy(data + sizeof(simpleProtoMsg.head), simpleProtoMsg.body.c_str(),
         simpleProtoMsg.body.size());
  SimpleProtoParser parser;
  parser.init();
  parser.parseMsg(data, len);
  std::cout << parser.msg_.body << std::endl;
}