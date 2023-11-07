#include <cstring>
#include "net/serverBase/include/http/HttpParser.h"
#include "common/include/Logger.h"
#include "common/include/StringUtil.h"
#include "net/serverBase/include/http/HttpRequest.h"

namespace CppUtil {
namespace Net {

void HttpHeader::set(std::string key, std::string value) {
  // Convert to UpperCamelCase, for example:
  //      transfer-encoding
  // transform to:
  //      Transfer-Encoding
  // char pchar = 0;
  // for (int i = 0; i < (int)key.length(); i++) {
  //   char ch = key.at(i);

  //   if (i == 0 || pchar == '-') {
  //     if (ch >= 'a' && ch <= 'z') {
  //       ((char*)key.data())[i] = ch - 32;
  //     }
  //   }
  //   pchar = ch;
  // }

  Common::strToLower(key);

  if (headers.find(key) == headers.end()) {
    keys_.emplace_back(key);
  }

  headers[key] = value;
}

std::string HttpHeader::get(std::string key) const {
  Common::strToLower(key);

  std::string v;
  auto it = headers.find(key);
  if (it != headers.end()) {
    v = it->second;
  }

  return v;
}

HttpParser::HttpParser()
    : type_(HTTP_REQUEST), headerPtr_(std::make_unique<HttpHeader>()) {}
HttpParser::~HttpParser() {}
void HttpParser::init(http_parser_type type) {
  type_ = type;
  http_parser_init(&parser_, type_);
  parser_.data = (void*)this;
  memset(&settings_, 0, sizeof(settings_));
  settings_.on_message_begin = on_message_begin;
  settings_.on_url = on_url;
  settings_.on_header_field = on_header_field;
  settings_.on_header_value = on_header_value;
  settings_.on_headers_complete = on_headers_complete;
  settings_.on_body = on_body;
  settings_.on_message_complete = on_message_complete;
  settings_.on_chunk_header = on_chunk_header;
}

Common::Status HttpParser::parseMsg(const char* buf, int len) {
  int wholeSize = len;
  while (wholeSize > 0) {
    ssize_t consumed = http_parser_execute(&parser_, &settings_, buf, len);
    LOG_DEBUG("consumed size = %ld", consumed)
    enum http_errno code = HTTP_PARSER_ERRNO(&parser_);
    if (code != HPE_OK) {
      return {(int)code, "http parser failed"};
    }
    wholeSize -= consumed;
  }

  return {};
}

void HttpParser::fillHttpRequest(HttpRequest& req) {
  req.setUrl(url_);
  // add more next
  req.setMethod(method_);
  req.setHttpHeader(*(headerPtr_.get()));
  req.setBody(body_);
  req.setMajorVer(httpMajor_);
  req.setMinorVer(httpMinor_);
  if (httpMajor_ == 1 && httpMinor_ == 0) {
    req.setVersion(HttpRequest::kHttp10);
  } else if (httpMajor_ == 1 && httpMinor_ == 1) {
    req.setVersion(HttpRequest::kHttp11);
  }
  std::string host = req.getHeader("Host");
  if (!host.empty()) {
    req.setHost(host);
  }
}

void HttpParser::reset() {
  url_.clear();
  method_ = HTTP_GET;
  state_ = kHttpParserInit;
  body_.clear();

  memset(&parser_, 0, sizeof(parser_));
  http_parser_init(&parser_, HTTP_BOTH);
  parser_.data = (void*)this;
  memset(&settings_, 0, sizeof(settings_));
  settings_.on_message_begin = on_message_begin;
  settings_.on_url = on_url;
  settings_.on_header_field = on_header_field;
  settings_.on_header_value = on_header_value;
  settings_.on_headers_complete = on_headers_complete;
  settings_.on_body = on_body;
  settings_.on_message_complete = on_message_complete;
  settings_.on_chunk_header = on_chunk_header;
}
bool HttpParser::parseFinish() const {
  if (state_ >= kHttpParserMessageComple) {
    return true;
  } else {
    return false;
  }
}

int HttpParser::on_message_begin(http_parser* parser) {
  HttpParser* obj = (HttpParser*)parser->data;
  obj->state_ = kHttpParserStart;
  // If we set to HTTP_BOTH, the type is detected and speicifed by parser.
  obj->parsed_type_ = (http_parser_type)parser->type;
  return 0;
}

int HttpParser::on_headers_complete(http_parser* parser) {
  HttpParser* obj = (HttpParser*)parser->data;
  obj->state_ = kHttpParserHeaderComplete;
  obj->httpMajor_ = parser->http_major;
  obj->httpMinor_ = parser->http_minor;
  LOG_DEBUG("http major = %u, minor %u", parser->http_major, parser->http_minor)
  LOG_DEBUG("header parse complete")
  return 0;
}

int HttpParser::on_message_complete(http_parser* parser) {
  HttpParser* obj = (HttpParser*)parser->data;
  obj->state_ = kHttpParserMessageComple;
  LOG_DEBUG("message parse complete")
  return 0;
}

int HttpParser::on_url(http_parser* parser, const char* at, size_t length) {
  HttpParser* obj = (HttpParser*)parser->data;
  if (length > 0) {
    obj->url_.append(at, length);
  }

  obj->method_ = parser->method;
  return 0;
}

int HttpParser::on_header_field(http_parser* parser, const char* at,
                                size_t length) {
  HttpParser* obj = (HttpParser*)parser->data;
  LOG_DEBUG("header field: %.*s", (int)length, at)
  if (length > 0) {
    obj->fieldName_.append(at, (int)length);
  }
  return 0;
}

int HttpParser::on_header_value(http_parser* parser, const char* at,
                                size_t length) {
  HttpParser* obj = (HttpParser*)parser->data;
  if (length > 0) {
    obj->fieldValue_.append(at, length);
  }

  if (!obj->fieldValue_.empty()) {
    obj->headerPtr_->set(obj->fieldName_, obj->fieldValue_);
    obj->fieldName_ = obj->fieldValue_ = "";
  }
  return 0;
}

int HttpParser::on_body(http_parser* parser, const char* at, size_t length) {
  HttpParser* obj = (HttpParser*)parser->data;
  obj->state_ = kHttpParserBody;
  obj->body_.append(at, length);
  LOG_DEBUG("on_body parse complete")
  return 0;
}

int HttpParser::on_chunk_header(http_parser* parser) {
  LOG_DEBUG("on_chunk_header, length = %lu", parser->content_length)
  return 0;
}

}  // namespace Net
}  // namespace CppUtil