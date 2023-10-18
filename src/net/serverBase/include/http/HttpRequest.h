#pragma once
#include <assert.h>
#include <stdio.h>

#include <map>
#include <string>

#include "common/include/Noncopyable.h"
#include "net/serverBase/include/http/HttpParser.h"
using std::string;
namespace CppUtil {
namespace Net {

class HttpRequest : public Noncopyable {
 public:
  enum Method { kInvalid, kGet, kPost, kHead, kPut, kDelete };
  enum Version { kUnknown, kHttp10, kHttp11 };
  using Http_Method = unsigned int;

  HttpRequest() : version_(kUnknown) {}

  void setVersion(Version v) { version_ = v; }

  Version getVersion() const { return version_; }

  void setPath(const char* start, const char* end) { path_.assign(start, end); }

  const string& path() const { return path_; }

  void setQuery(const char* start, const char* end) {
    query_.assign(start, end);
  }

  const string& query() const { return query_; }

  void swap(HttpRequest& that) {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    // receiveTime_.swap(that.receiveTime_);
    std::swap(httpHeader_, that.httpHeader_);
  }
  void setUrl(const std::string& url) { url_ = url; }
  void setMethod(Http_Method method) { method_ = method; }
  std::string getUrl() const { return url_; }
  std::string getMethodStr() const;

  void setHttpHeader(HttpHeader httpHeader) {
    httpHeader_ = std::move(httpHeader);
  }

  std::string getHeader(const std::string& key) const {
    return httpHeader_.get(key);
  }
  void setBody(std::string& body) { body_ = std::move(body); }
  std::string getBody() const { return body_; }
  void setMajorVer(unsigned int majorVersion) { httpMajor_ = majorVersion; }
  unsigned int getMajorVer() const { return httpMajor_; }
  unsigned int getMinorVer() const { return httpMinor_; }
  void setMinorVer(unsigned int minorVersion) { httpMinor_ = minorVersion; }
  void setHost(std::string host) { host_ = host; }
  std::string getHost() const { return host_; }

 private:
  Http_Method method_;
  Version version_;
  string path_;
  string query_;
  std::string url_;
  // Timestamp receiveTime_;
  HttpHeader httpHeader_;
  std::string body_;
  unsigned short httpMajor_;
  unsigned short httpMinor_;
  std::string host_;
};

}  // namespace Net
}  // namespace CppUtil
