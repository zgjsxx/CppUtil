
#include "net/serverBase/include/EventLoop.h"

#include <sys/eventfd.h>

#include "common/include/Thread.h"
#include "net/serverBase/include/EpollMonitor.h"
#include "net/serverBase/include/SocketUtil.h"

namespace CppUtil {

namespace Net {

const int kPollTimeMs = 1000;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop()
    : threadId_(getCurrentTid()),
      sockMonitorPtr_(new EpollMonitor(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      callingPendingFunctors_(false) {
  wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
  ::close(wakeupFd_);
  wakeupChannel_->disableAll();
  wakeupChannel_->remove();
}

void EventLoop::loop() {
  looping_ = true;
  quit_ = false;
  while (!quit_) {
    activeChannels_.clear();
    sockMonitorPtr_->poll(kPollTimeMs, &activeChannels_);
    eventHandling_ = true;
    for (auto& channel : activeChannels_) {
      currentActiveChannel_ = channel;
      currentActiveChannel_->handleEvent();
    }
    currentActiveChannel_ = nullptr;
    eventHandling_ = false;
    doPendingFunctors();
  }
  looping_ = false;
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
    std::unique_lock<std::mutex> lk(mtx_);
    functors.swap(pendingFunctors_);
  }

  for (const Functor& functor : functors) {
    functor();
  }
  callingPendingFunctors_ = false;
}

void EventLoop::runInLoop(const Functor& cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::updateChannel(Channel* channel) {
  sockMonitorPtr_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
  sockMonitorPtr_->removeChannel(channel);
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = SockUtil::read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
  }
}

void EventLoop::queueInLoop(const Functor& cb) {
  {
    std::unique_lock<std::mutex> lk(mtx_);
    pendingFunctors_.push_back(std::move(cb));
  }

  if (!isInLoopThread() || callingPendingFunctors_) {
    wakeup();
  }
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = SockUtil::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof(one)) {
  }
}

bool EventLoop::hasChannel(Channel* channel) {
  return sockMonitorPtr_->hasChannel(channel);
}

}  // namespace Net
}  // namespace CppUtil