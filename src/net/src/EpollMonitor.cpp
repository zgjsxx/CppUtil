#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
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

void EpollMonitor::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_, 
                        &*events_.begin(),
                        static_cast<int>(events_.size()),
                        timeoutMs);
    int savedErrno = errno;
    if(numEvents > 0){
        fillActiveChannels(numEvents, activeChannels);
        if((size_t)numEvents == events_.size()){
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0){
        //no event happens
    }
    else{
        if(savedErrno != EINTR){
            errno = savedErrno;
        }
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