#ifndef CPPUTIL_NET_EVENTLOOP_H
#define CPPUTIL_NET_EVENTLOOP_H
#include <mutex>

#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace net {

class EventLoop : public Noncopyable {
 public:
  EventLoop();
  ~EventLoop();

 public:
  void loop();

 private:
  mutable std::mutex mtx_;
};

}  // namespace net

}  // namespace CppUtil

#endif