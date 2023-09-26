#ifndef CPP_UTIL_COMMON_THREADPOOL_H
#define CPP_UTIL_COMMON_THREADPOOL_H

#include <memory>
#include <string>
#include <functional>
#include "common/include/Noncopyable.h"

namespace CppUtil
{

class ThreadPool : public Noncopyable
{
public:
  using Task = std::function<void ()>;
  explicit ThreadPool(const std::string& name = "DefaultThreadPool");
  ~ThreadPool();

  void start(int num);

   void postTask(Task task);
   Task getTask();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_{nullptr};
};

}


#endif