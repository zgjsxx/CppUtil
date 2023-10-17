#pragma once

#include <string>

namespace CppUtil {

namespace Common {
class Status {
 public:
  Status() = default;
  Status(int code, const std::string& msg) : code_(code), msg_(msg) {}

 public:
  [[nodiscard]] inline bool ok() const { return code_ == 0; }
  [[nodiscard]] inline int code() const { return code_; }
  [[nodiscard]] inline std::string message() const { return msg_; }

 private:
  int code_{0};
  std::string msg_;
};

}  // namespace Common
}  // namespace CppUtil
