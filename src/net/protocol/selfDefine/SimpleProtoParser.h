#pragma once

#include <stdint.h>
#include <vector>
#include <string>

const uint8_t MY_PROTO_MAGIC = 88;
const uint32_t MY_PROTO_MAX_SIZE = 10 * 1024 * 1024;  // 10M
const uint32_t MY_PROTO_HEAD_SIZE = 8;

enum MyProtoParserStatus {
  ON_PARSER_INIT = 0,
  ON_PARSER_HAED_FINISH = 1,
  ON_PARSER_BODY_FINISH = 2,
};

struct SimpleProtoHeader {
  uint16_t version;
  uint16_t magic;
  uint32_t len;
};

struct SimpleProtoMsg {
  SimpleProtoHeader head;
  std::string body;
};

class SimpleProtoParser {
 public:
  void init();
  void clear();
  void parseMsg(void *data, size_t len);
  bool parseFinish() const { return parserStatus_ >= ON_PARSER_BODY_FINISH; }
  SimpleProtoMsg msg_;

 private:
  void parseHeader();
  void parseBody();

 private:
  std::vector<uint8_t> reserved_;
  MyProtoParserStatus parserStatus_;

  uint8_t *curData_{nullptr};
};