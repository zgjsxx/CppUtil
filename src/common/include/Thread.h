#pragma once

#include <sys/syscall.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>

#include "common/include/Noncopyable.h"

namespace CppUtil {

extern thread_local int t_tid;

inline pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }

inline int getCurrentTid() {
  if (t_tid == 0) {
    t_tid = ::gettid();
  }
  return t_tid;
}

class Thread : public Noncopyable {
 public:
  using ThreadFunc = std::function<void()>;
  explicit Thread(ThreadFunc th, const std::string& name);
  ~Thread();

  void start();
  void stop();

  bool started() const;
  pid_t tid() const;

  const std::string& name() const;
  void exec();

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace CppUtil
