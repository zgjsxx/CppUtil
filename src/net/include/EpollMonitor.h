#ifndef CPPUTIL_NET_MONITOR_H
#define CPPUTIL_NET_MONITOR_H

#include <map>
#include <vector>
#include <sys/epoll.h>
#include "net/include/Channel.h"

namespace CppUtil {

namespace Net {

class EventLoop;
class ChannelList;

class EpollMonitor {
 public:
  using ChannelList = std::vector<Channel*>;
  EpollMonitor(EventLoop* loop);
  ~EpollMonitor();

  void poll(int timeoutMs, ChannelList* activeChannels);
  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
  // add event/delete event
  void updateInterestEvent(int operation, Channel* channel);
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  bool hasChannel(Channel* channel) const;

 private:
  using EventList = std::vector<struct epoll_event>;
  static const int kInitEventListSize = 16;
  int epollfd_;
  EventList events_;
  using ChannelMap = std::map<int, Channel*>;
  ChannelMap channels_;
};

}  // namespace Net
}  // namespace CppUtil
#endif