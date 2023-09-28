#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include "net/include/EpollMonitor.h"


namespace CppUtil
{

namespace Net
{
EpollMonitor::EpollMonitor(EventLoop* loop)
{

}

EpollMonitor::~EpollMonitor()
{
    ::close(epollfd_);
}

void EpollMonitor::poll(int timeoutMs)
{
    int numEvents = ::epoll_wait(epollfd_, 
                        &*events_.begin(),
                        static_cast<int>(events_.size()),
                        timeoutMs);
    if(numEvents > 0){
        //fillActiveEvents
    }
    else if(numEvents == 0){
        //no event happens
    }
    else{

    }
}

void EpollMonitor::fillActiveChannels(int numEvents,
                                     ChannelList* activeChannels) const
{
  for (int i = 0; i < numEvents; ++i)
  {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->setRevents(events_[i].events);
    activeChannels->emplace_back(channel);
  }
}

void EpollMonitor::updateInterestEvent(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int fd = channel->getFd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        //add log
    }
}

} // namespace Net
} // namespace CppUtil