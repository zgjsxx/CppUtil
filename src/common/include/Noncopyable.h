#pragma once
namespace CppUtil {

class Noncopyable {
 public:
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;

 protected:
  Noncopyable() = default;
  ~Noncopyable() = default;
};

}  // namespace CppUtil
