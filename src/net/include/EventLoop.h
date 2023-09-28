#ifndef CPPUTIL_NET_EVENTLOOP_H
#define CPPUTIL_NET_EVENTLOOP_H
#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace Net {

class Channel;
class EpollMonitor;

class EventLoop : public Noncopyable {
 public:
  EventLoop();
  ~EventLoop();

 public:
  void loop();

 private:
  using ChannelList =  std::vector<Channel*>;
  mutable std::mutex mtx_;
  std::atomic<bool> quit_{false};
  ChannelList activeChannels_;
  std::unique_ptr<EpollMonitor> sockMonitorPtr_;
  Channel* currentActiveChannel_{nullptr};
  int wakeupFd_{-1};
  int threadId_{-1};
};

}  // namespace net

}  // namespace CppUtil

#endif