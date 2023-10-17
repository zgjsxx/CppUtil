#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace CppUtil {

class CountDownLatch {
 public:
  explicit CountDownLatch(const size_t count);
  ~CountDownLatch() = default;

  void wait();

  template <class Rep, class Period>
  bool wait(const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lk(mtx_);
    bool result = true;
    if (count_ > 0) {
      result = cv_.wait(lk, timeout, [this]() { return count_ == 0; });
    }
    return result;
  }

  void countDown();

  size_t getCount();

  void setCount(int num);

 private:
  std::mutex mtx_;
  std::condition_variable cv_;
  size_t count_{0};
};
}  // namespace CppUtil