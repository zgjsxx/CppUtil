#ifndef CPP_UTIL_COMMON_THREADPOOL_H
#define CPP_UTIL_COMMON_THREADPOOL_H

#include <functional>
#include <memory>
#include <string>

#include "common/include/Noncopyable.h"

namespace CppUtil {

class ThreadPool : public Noncopyable {
 public:
  using Task = std::function<void()>;
  explicit ThreadPool(const std::string& name = "DefaultThreadPool");
  ~ThreadPool();

  void start(int num);
  void stop();
  void stopSafe();
  void postTask(Task task);
  Task getTask();
  void execTask();

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_{nullptr};
};

}  // namespace CppUtil

#endif