#pragma once

#include <string>
#include "net/protocol/websocket/websocket_parser.h"

namespace CppUtil {
namespace Net {
class WebSocketParser {
 public:
  enum State { start = 0, header = 1, body = 2, complete = 3 };

  WebSocketParser();
  ~WebSocketParser();

 public:
  void init();
  bool parseMsg(const char *buf, int len);
  bool parseFinish() const;
  std::string getBody() const { return body_; }

 private:
  static int on_frame_header(websocket_parser *parser);
  static int on_frame_body(websocket_parser *parser, const char *at,
                           size_t size);
  static int on_frame_end(websocket_parser *parser);

 private:
  websocket_parser_settings settings_;
  websocket_parser parser_;
  int opcode_{-1};
  bool isFinal_{false};
  std::string body_;
  State state_{start};
};
}  // namespace Net
}  // namespace CppUtil