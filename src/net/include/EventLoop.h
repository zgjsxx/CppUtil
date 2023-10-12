#ifndef CPPUTIL_NET_EVENTLOOP_H
#define CPPUTIL_NET_EVENTLOOP_H
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace Net {

class Channel;
class EpollMonitor;

class EventLoop : public Noncopyable {
 public:
  using Functor = std::function<void()>;
  EventLoop();
  ~EventLoop();

 public:
  void loop();
  void runInLoop(const Functor& cb);
  void quit();
  void queueInLoop(const Functor& cb);
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  void wakeup();
  void handleRead();  // waked up
  bool hasChannel(Channel* channel);

 private:
  using ChannelList = std::vector<Channel*>;
  mutable std::mutex mtx_;
  std::atomic<bool> quit_{false};
  ChannelList activeChannels_;
  std::unique_ptr<EpollMonitor> sockMonitorPtr_;
  Channel* currentActiveChannel_{nullptr};
  int wakeupFd_{-1};
  int threadId_{-1};
};

}  // namespace Net

}  // namespace CppUtil

#endif