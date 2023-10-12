#include "net/include/EventLoopThreadPool.h"

#include "net/include/EventLoop.h"
#include "net/include/EventLoopThread.h"

namespace CppUtil {
namespace Net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop,
                                         const std::string& name)
    : baseLoop_(baseloop),
      name_(name),
      started_(false),
      numThreads_(0),
      next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {
  // no need to delete baseLoop_, its a borrow pointer, not owned by
  // EventLoopThreadPool
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
  started_ = true;
  for (int i = 0; i < numThreads_; ++i) {
    char buf[name_.size() + 32] = {0};
    snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
    EventLoopThread* t = new EventLoopThread(cb, buf);
    threads_.emplace_back(std::unique_ptr<EventLoopThread>(t));
    loops_.emplace_back(t->startLoop());
  }

  if (numThreads_ == 0 && cb) {
    cb(baseLoop_);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
  EventLoop* loop = baseLoop_;

  if (!loops_.empty()) {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if ((size_t)(next_) >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode) {
  EventLoop* loop = baseLoop_;

  if (!loops_.empty()) {
    loop = loops_[hashCode % loops_.size()];
  }
  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
  if (loops_.empty()) {
    return std::vector<EventLoop*>(1, baseLoop_);
  } else {
    return loops_;
  }
}

}  // namespace Net
}  // namespace CppUtil
