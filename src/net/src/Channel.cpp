#include "net/include/Channel.h"

#include "net/include/EventLoop.h"

namespace CppUtil {

namespace Net {
// EPOLLIN = 0x001,
// EPOLLPRI = 0x002,
// EPOLLOUT = 0x004,
// EPOLLRDNORM = 0x040,
// EPOLLRDBAND = 0x080,
// EPOLLWRNORM = 0x100,
// EPOLLWRBAND = 0x200,
// EPOLLMSG = 0x400,
// EPOLLERR = 0x008,
// EPOLLHUP = 0x010,
// EPOLLRDHUP = 0x2000,
// EPOLLEXCLUSIVE = 1u << 28,
// EPOLLWAKEUP = 1u << 29,
// EPOLLONESHOT = 1u << 30,
// EPOLLET = 1u << 31

// #define POLLIN		0x001		/* There is data to read.  */
// #define POLLPRI		0x002		/* There is urgent data to read.
// */ #define POLLOUT		0x004		/* Writing now will not block.
// */

// # define POLLRDNORM	0x040		/* Normal data may be read.  */
// # define POLLRDBAND	0x080		/* Priority data may be read.  */
// # define POLLWRNORM	0x100		/* Writing now will not block.  */
// # define POLLWRBAND	0x200		/* Priority data may be written.  */

// # define POLLMSG	0x400
// # define POLLREMOVE	0x1000
// # define POLLRDHUP	0x2000

// #define POLLERR		0x008		/* Error condition.  */
// #define POLLHUP		0x010		/* Hung up.  */
// #define POLLNVAL	0x020		/* Invalid polling request.  */

Channel::Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd), index_(-1) {}
void Channel::handleEvent() {
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
    // close event
    if (closeCallback_) {
      closeCallback_();
    }
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    // error event
    if (errorCallback_) {
      errorCallback_();
    }
  }

  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    // read event
    if (readCallback_) {
      readCallback_();
    }
  }

  if (revents_ & POLLOUT) {
    // write event
    if (writeCallback_) {
      writeCallback_();
    }
  }
}

void Channel::update() {
  addedToLoop_ = true;
  loop_->updateChannel(this);
}

void Channel::remove() {
  addedToLoop_ = false;
  loop_->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void>& obj) {
  // obj is TcpConnection type
  tie_ = obj;
  tied_ = true;
}

}  // namespace Net
}  // namespace CppUtil