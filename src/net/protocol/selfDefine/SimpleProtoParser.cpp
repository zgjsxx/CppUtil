#include "net/protocol/selfDefine/SimpleProtoParser.h"

void SimpleProtoParser::init() { parserStatus_ = ON_PARSER_INIT; }

void SimpleProtoParser::clear() {
  reserved_.clear();
  curData_ = nullptr;
  parserStatus_ = ON_PARSER_INIT;
}

void SimpleProtoParser::parseHeader() {
  uint8_t *endData_ = (uint8_t *)&*reserved_.end();
  if (endData_ - curData_ < SIMPLE_PROTO_HEAD_SIZE) {
    return;
  }

  msg_.head.len = *(uint32_t *)curData_;
  curData_ += 4;

  parserStatus_ = ON_PARSER_HAED_FINISH;
  return;
}

void SimpleProtoParser::parseBody() {
  uint32_t bodySize = msg_.head.len;
  uint8_t *endData_ = (uint8_t *)&*reserved_.end();
  if (endData_ - curData_ < bodySize) {
    return;
  }
  msg_.body.append((char *)curData_, bodySize);
  parserStatus_ = ON_PARSER_BODY_FINISH;

  return;
}

void SimpleProtoParser::parseMsg(void *data, size_t len) {
  if (len <= 0) {
    return;
  }

  curData_ = (uint8_t *)data;
  while (len--) {
    reserved_.push_back(*curData_);
    ++curData_;
  }

  curData_ = (uint8_t *)&reserved_[0];

  if (ON_PARSER_INIT == parserStatus_) {
    parseHeader();
  }

  if (ON_PARSER_HAED_FINISH == parserStatus_) {
    parseBody();
  }
  return;
}
