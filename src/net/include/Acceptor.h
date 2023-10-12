#pragma once

#include <functional>

#include "net/include/Channel.h"
#include "net/include/Socket.h"

namespace CppUtil {
namespace Net {

class EventLoop;
class InetAddress;

///
/// Acceptor of incoming TCP connections.
///
class Acceptor : public Noncopyable {
 public:
  using NewConnCallback = std::function<void(int sockfd, const InetAddress&)>;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnCallback& cb) {
    newConnCallback_ = cb;
  }

  void listen();

  bool listening() const { return listening_; }

  // Deprecated, use the correct spelling one above.
  // Leave the wrong spelling here in case one needs to grep it for error
  // messages. bool listenning() const { return listening(); }

 private:
  void handleRead();

  // Acceptor run in basic eventloop
  EventLoop* loop_{nullptr};
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnCallback newConnCallback_;
  bool listening_;
  int idleFd_;
};

}  // namespace Net
}  // namespace CppUtil
