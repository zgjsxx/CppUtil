#pragma once

#include <functional>

#include "net/serverBase/include/Channel.h"
#include "net/serverBase/include/Socket.h"

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

 private:
  void handleRead();

  // Acceptor run in basic eventloop
  EventLoop* loop_{nullptr};
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnCallback newConnCallback_;
  bool listening_{false};
  int idleFd_{-1};
};

}  // namespace Net
}  // namespace CppUtil
