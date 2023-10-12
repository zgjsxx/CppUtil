#include "net/include/EpollMonitor.h"

#include <errno.h>

#include <unistd.h>

#include <cstring>

namespace CppUtil {

namespace Net {

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollMonitor::EpollMonitor(EventLoop* loop) {}

EpollMonitor::~EpollMonitor() { ::close(epollfd_); }

void EpollMonitor::poll(int timeoutMs, ChannelList* activeChannels) {
  int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeoutMs);
  int savedErrno = errno;
  if (numEvents > 0) {
    fillActiveChannels(numEvents, activeChannels);
    if ((size_t)numEvents == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
    // no event happens
  } else {
    if (savedErrno != EINTR) {
      errno = savedErrno;
    }
  }
}

void EpollMonitor::fillActiveChannels(int numEvents,
                                      ChannelList* activeChannels) const {
  for (int i = 0; i < numEvents; ++i) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->setRevents(events_[i].events);
    activeChannels->emplace_back(channel);
  }
}

void EpollMonitor::updateInterestEvent(int operation, Channel* channel) {
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.events = channel->getEvents();
  event.data.ptr = channel;
  int fd = channel->getFd();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    // add log
  }
}

void EpollMonitor::updateChannel(Channel* channel) {
  const int index = channel->index();
  if (index == kNew || index == kDeleted) {
    int fd = channel->getFd();
    if (index == kNew) {
      channels_[fd] = channel;
    }

    channel->setIndex(kAdded);
    updateInterestEvent(EPOLL_CTL_ADD, channel);
  } else {
    // update existing one with EPOLL_CTL_MOD/DEL

    if (channel->isNoneEvent()) {
      updateInterestEvent(EPOLL_CTL_DEL, channel);
      channel->setIndex(kDeleted);
    } else {
      updateInterestEvent(EPOLL_CTL_MOD, channel);
    }
  }
}

void EpollMonitor::removeChannel(Channel* channel) {
  int fd = channel->getFd();
  int index = channel->index();
  size_t n = channels_.erase(fd);
  (void)n;

  if (index == kAdded) {
    updateInterestEvent(EPOLL_CTL_DEL, channel);
  }
  channel->setIndex(kNew);
}

bool EpollMonitor::hasChannel(Channel* channel) const {
  ChannelMap::const_iterator it = channels_.find(channel->getFd());
  return it != channels_.end() && it->second == channel;
}

}  // namespace Net
}  // namespace CppUtil