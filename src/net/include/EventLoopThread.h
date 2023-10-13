#ifndef CPPUTIL_NET_EVENTLOOPTHREAD_H
#define CPPUTIL_NET_EVENTLOOPTHREAD_H

#include <condition_variable>
#include <functional>
#include <mutex>

#include "common/include/Noncopyable.h"
#include "common/include/Thread.h"

namespace CppUtil {

namespace Net {

class EventLoop;

class EventLoopThread : public Noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;
  EventLoopThread(const ThreadInitCallback& bk, const std::string& name);
  ~EventLoopThread();
  EventLoop* waitThreadStart();

 private:
  void threadFunc();

  EventLoop* loop_{nullptr};
  bool exiting_;
  Thread thread_;
  std::mutex mtx_;
  std::condition_variable cond_;
  ThreadInitCallback callback_;
};

}  // namespace Net

}  // namespace CppUtil

#endif