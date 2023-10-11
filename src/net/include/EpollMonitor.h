#ifndef CPPUTIL_NET_MONITOR_H
#define CPPUTIL_NET_MONITOR_H

#include <vector>
#include <map>
#include "net/include/Channel.h"

namespace CppUtil
{

namespace Net
{

class EventLoop;
class ChannelList;

class EpollMonitor {
 public:

 using ChannelList = std::vector<Channel*> ;
  EpollMonitor(EventLoop* loop);
  ~EpollMonitor();

  void poll(int timeoutMs, ChannelList* activeChannels);
  void fillActiveChannels(int numEvents,
                              ChannelList* activeChannels) const;
   //add event/delete event
  void updateInterestEvent(int operation, Channel* channel);
  void updateChannel(Channel* channel);
 private:
  using EventList = std::vector<struct epoll_event>;
  int epollfd_;
  EventList events_;
  using ChannelMap = std::map<int, Channel*> ;
  ChannelMap channels_;
};


}
}
#endif