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

  void enableReading() { events_ |= kReadEvent; update(); }
  void disableReading() { events_ &= ~kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
  void disableAll() { events_ = kNoneEvent; update(); }


  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }
  
  // update interest event in evet loop
  void update();
  int index() { return index_; }
  void setIndex(int idx) { index_ = idx; }

  void setRevents(int revent) { revents_ = revent; } // used by pollers

    EventLoop* ownerLoop() { return loop_; }
    void remove();
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
  int index_{-1}; // used by Poller.
  int events_;   // event that we care about
  int revents_;  // ready event that we get
};


}
}  // namespace CppUtil
#endif