#include "net/include/EventLoop.h"
#include "net/include/EpollMonitor.h"
#include "common/include/Thread.h"

namespace CppUtil
{

namespace Net
{

EventLoop::EventLoop()
    :threadId_(getCurrentTid())
{

}

EventLoop::~EventLoop()
{

}
void EventLoop::loop()
{
    while(!quit_){
        activeChannels_.clear();
        sockMonitorPtr_->poll(100);
        for(auto& channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent();
        }
        currentActiveChannel_ = nullptr;
    }
}

}

}