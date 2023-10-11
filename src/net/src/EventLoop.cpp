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
        sockMonitorPtr_->poll(100, &activeChannels_);
        for(auto& channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent();
        }
        currentActiveChannel_ = nullptr;
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    cb();
}

void EventLoop::updateChannel(Channel* channel)
{
    sockMonitorPtr_->updateChannel(channel);
}


}
}