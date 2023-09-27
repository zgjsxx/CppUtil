#include <sys/epoll.h>
#include <unistd.h>
#include "net/include/EpollMonitor.h"



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
}