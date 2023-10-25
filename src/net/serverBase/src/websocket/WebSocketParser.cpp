#include "net/serverBase/include/websocket/WebSocketParser.h"

namespace CppUtil {
namespace Net {

WebSocketParser::WebSocketParser() {}
WebSocketParser::~WebSocketParser() {}

bool WebSocketParser::parseMsg(const char *buf, int len) {
  int wholeSize = len;
  while (wholeSize > 0) {
    ssize_t consumed = websocket_parser_execute(&parser_, &settings_, buf, len);
    // LOG_DEBUG("consumed size = %d", consumed)
    wholeSize -= consumed;
  }
  return true;
}

bool WebSocketParser::parseFinish() const {
  if (state_ >= State::complete) {
    return true;
  }
  return false;
}

int WebSocketParser::on_frame_header(websocket_parser *parser) {
  WebSocketParser *obj = (WebSocketParser *)parser->data;

  obj->opcode_ = parser->flags & WS_OP_MASK;  // gets opcode
  obj->isFinal_ = parser->flags & WS_FIN;     // checks is final frame
  if (parser->length) {
    obj->body_.reserve(
        parser->length);  // allocate memory for frame body, if body exists
  }
  obj->state_ = State::header;
  return 0;
}
int WebSocketParser::on_frame_body(websocket_parser *parser, const char *at,
                                   size_t size) {
  WebSocketParser *obj = (WebSocketParser *)parser->data;

  if (parser->flags & WS_HAS_MASK) {
    // if frame has mask, we have to copy and decode data via
    // websocket_parser_copy_masked function
    char *data = new char[size];
    websocket_parser_decode(data, at, size, parser);
    obj->body_.append(data, size);
    delete[] data;
  } else {
    obj->body_.append(at, size);
  }
  obj->state_ = State::body;
  return 0;
}

int WebSocketParser::on_frame_end(websocket_parser *parser) {
  WebSocketParser *obj = (WebSocketParser *)parser->data;

  obj->state_ = State::complete;
  return 0;
}

void WebSocketParser::init() {
  websocket_parser_settings_init(&settings_);
  settings_.on_frame_header = on_frame_header;
  settings_.on_frame_body = on_frame_body;
  settings_.on_frame_end = on_frame_end;
  websocket_parser_init(&parser_);
  parser_.data = (void *)this;
}

}  // namespace Net
}  // namespace CppUtil