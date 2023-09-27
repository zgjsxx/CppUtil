#ifndef CPPUTIL_NET_MONITOR_H
#define CPPUTIL_NET_MONITOR_H

#include <vector>

class EventLoop;
class ChannelList;

class EpollMonitor {
 public:
  EpollMonitor(EventLoop* loop);
  ~EpollMonitor();

  void poll(int timeoutMs);
 private:
  using EventList = std::vector<struct epoll_event>;
  int epollfd_;
  EventList events_;
};

#endif