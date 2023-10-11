#include "net/include/EventLoop.h"
#include "net/include/EpollMonitor.h"
#include "net/include/SocketUtil.h"
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

void EventLoop::removeChannel(Channel* channel)
{
  sockMonitorPtr_->removeChannel(channel);
}

void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = SockUtil::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof(one))
  {

  }
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = SockUtil::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof(one))
  {

  }
}

bool EventLoop::hasChannel(Channel* channel)
{
  return sockMonitorPtr_->hasChannel(channel);
}


}
}