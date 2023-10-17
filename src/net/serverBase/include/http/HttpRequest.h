#pragma once
#include <assert.h>
#include <stdio.h>

#include <map>
#include <string>

#include "common/include/Noncopyable.h"
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

  // void setReceiveTime(Timestamp t)
  // { receiveTime_ = t; }

  // Timestamp receiveTime() const
  // { return receiveTime_; }

  void addHeader(const char* start, const char* colon, const char* end) {
    string field(start, colon);
    ++colon;
    while (colon < end && isspace(*colon)) {
      ++colon;
    }
    string value(colon, end);
    while (!value.empty() && isspace(value[value.size() - 1])) {
      value.resize(value.size() - 1);
    }
    headers_[field] = value;
  }

  string getHeader(const string& field) const {
    string result;
    std::map<string, string>::const_iterator it = headers_.find(field);
    if (it != headers_.end()) {
      result = it->second;
    }
    return result;
  }

  const std::map<string, string>& headers() const { return headers_; }

  void swap(HttpRequest& that) {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    // receiveTime_.swap(that.receiveTime_);
    headers_.swap(that.headers_);
  }
  void setUrl(const std::string& url) { url_ = url; }
  void setMethod(Http_Method method) { method_ = method; }
  std::string getUrl() const { return url_; }
  std::string getMethodStr() const;

 private:
  Http_Method method_;
  Version version_;
  string path_;
  string query_;
  std::string url_;
  // Timestamp receiveTime_;
  std::map<string, string> headers_;
};

}  // namespace Net
}  // namespace CppUtil
