#pragma once

#include <functional>
namespace CppUtil {
namespace Common {
class ScopeExit {
 public:
  ScopeExit() = default;
  inline ~ScopeExit() { exitFunc_(); };
  ScopeExit(const ScopeExit&) = delete;
  ScopeExit& operator=(const ScopeExit&) = delete;
  inline void setExitFunc(std::function<void()> exitFunc) {
    exitFunc_ = exitFunc;
  }

 private:
  std::function<void()> exitFunc_;
};
}  // namespace Common
}  // namespace CppUtil