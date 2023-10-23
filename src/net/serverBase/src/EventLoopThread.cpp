#include "net/serverBase/include/EventLoopThread.h"

#include "net/serverBase/include/EventLoop.h"

namespace CppUtil {

namespace Net {

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name)
    : loop_{nullptr},
      exiting_{false},
      thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      callback_(cb) {}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  if (loop_ != nullptr) {
    loop_->quit();
    thread_.stop();
  }
}

EventLoop* EventLoopThread::waitThreadStart() {
  thread_.start();
  EventLoop* loop = nullptr;
  {
    std::unique_lock<std::mutex> lk(mtx_);
    cond_.wait(lk, [this]() { return loop_ != nullptr; });
    loop = loop_;
  }
  return loop;
}

void EventLoopThread::stop() { loop_->quit(); }

void EventLoopThread::threadFunc() {
  EventLoop loop;
  if (callback_) {
    callback_(&loop);
  }

  {
    std::unique_lock<std::mutex> lk(mtx_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.loop();

  {
    std::unique_lock<std::mutex> lk(mtx_);
    loop_ = nullptr;
  }
}

}  // namespace Net
}  // namespace CppUtil