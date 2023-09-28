#ifndef CPPUTIL_NET_MONITOR_H
#define CPPUTIL_NET_MONITOR_H

#include <vector>
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

  void poll(int timeoutMs);
  void fillActiveChannels(int numEvents,
                              ChannelList* activeChannels) const;
   //add event/delete event
  void updateInterestEvent(int operation, Channel* channel);
 private:
  using EventList = std::vector<struct epoll_event>;
  int epollfd_;
  EventList events_;
};


}
}
#endif