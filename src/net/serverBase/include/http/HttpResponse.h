#pragma once

#include <map>
#include <string>
#include "common/include/Noncopyable.h"
#include "net/protocol/http/http_parser.h"
namespace CppUtil {
namespace Net {

class Buffer;
class HttpResponse : public Noncopyable {
 public:
  explicit HttpResponse(bool close)
      : statusCode_(HTTP_STATUS_OK), closeConnection_(close) {}

  void setStatusCode(http_status code) { statusCode_ = code; }

  void setCloseConnection(bool on) { closeConnection_ = on; }

  bool closeConnection() const { return closeConnection_; }

  void setContentType(const std::string& contentType) {
    addHeader("Content-Type", contentType);
  }

  // FIXME: replace string with StringPiece
  void addHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }

  void setBody(const std::string& body) { body_ = body; }

  void appendToBuffer(Buffer* output) const;

 private:
  std::map<std::string, std::string> headers_;
  // FIXME: add http version
  bool closeConnection_;
  std::string body_;
  http_status statusCode_;
};

}  // namespace Net
}  // namespace CppUtil