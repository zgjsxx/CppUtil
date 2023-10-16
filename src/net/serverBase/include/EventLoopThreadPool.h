#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace Net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : public Noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;
  EventLoopThreadPool(EventLoop* baseloop, const std::string& name);
  ~EventLoopThreadPool();
  void start(const ThreadInitCallback& cb);
  EventLoop* getNextLoop();
  EventLoop* getLoopForHash(size_t hashCode);
  std::vector<EventLoop*> getAllLoops();
  void setThreadNum(int numThreads) { numThreads_ = numThreads; }

 private:
  EventLoop* baseLoop_;
  std::string name_;
  int numThreads_;
  bool started_;
  int next_;
  std::vector<std::unique_ptr<EventLoopThread> > threads_;
  std::vector<EventLoop*> loops_;
};

}  // namespace Net

}  // namespace CppUtil