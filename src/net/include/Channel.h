#ifndef CPPUTIL_NET_CHANNEL_H
#define CPPUTIL_NET_CHANNEL_H

#include <functional>
#include <poll.h>
#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace Net
{

class EventLoop;

constexpr int kNoneEvent = 0;
constexpr int kReadEvent = POLLIN | POLLPRI;
constexpr int kWriteEvent = POLLOUT;

class Channel : public Noncopyable {
 public:
  using EventCallback = std::function<void()>;
  Channel(EventLoop* loop, int fd);

 public:
  void setReadCallback(EventCallback cb) { readCallback_ = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

  int getFd() const { return fd_; }

  int getEvents() const {return events_;}

  bool isNoneEvent() const { return events_ == kNoneEvent; }

  // update interest event in evet loop
  void update();

  void setRevents(int revent) { revents_ = revent; } // used by pollers
public:
  void handleEvent();
 private:
  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
  EventLoop* loop_;
  bool addedToLoop_{false};
  int fd_;
  int events_;   // event that we care about
  int revents_;  // ready event that we get
};


}
}  // namespace CppUtil
#endif