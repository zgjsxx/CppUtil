#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "common/include/Status.h"
#include "net/protocol/http/http_parser.h"

namespace CppUtil {
namespace Net {

class HttpRequest;

enum HttpParserState {
  kHttpParserInit = 0,
  kHttpParserStart,
  kHttpParserHeaderComplete,
  kHttpParserBody,
  kHttpParserMessageComple
};

class HttpHeader {
 public:
  HttpHeader() = default;
  ~HttpHeader() = default;
  void set(std::string key, std::string value);

  std::string get(std::string key);

 private:
  // FIXME:Set-Cookie can be duplicated in http header
  std::map<std::string, std::string> headers;
  std::vector<std::string> keys_;
};

class HttpParser {
 public:
  HttpParser();
  ~HttpParser();

 public:
  void init(http_parser_type type);
  Common::Status parseMsg(const char* buf, int len);
  bool parseFinish() const;
  void fillHttpRequest(HttpRequest& req);
  void reset();

 private:
  static int on_message_begin(http_parser* parser);
  static int on_headers_complete(http_parser* parser);
  static int on_message_complete(http_parser* parser);
  static int on_url(http_parser* parser, const char* at, size_t length);
  static int on_header_field(http_parser* parser, const char* at,
                             size_t length);
  static int on_header_value(http_parser* parser, const char* at,
                             size_t legnth);
  static int on_body(http_parser* parser, const char* at, size_t length);

 private:
  http_parser_settings settings_;
  http_parser parser_;
  std::string fieldName_;
  std::string fieldValue_;
  HttpParserState state_;
  http_parser_type type_;  // HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH
  http_parser_type parsed_type_;
  std::string url_;
  std::unique_ptr<HttpHeader> headerPtr_;
  std::string body_;
};

}  // namespace Net
}  // namespace CppUtil
