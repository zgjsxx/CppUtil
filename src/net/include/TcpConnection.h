#pragma once

#include <any>
#include <memory>
#include <string>

#include "common/include/Noncopyable.h"
#include "net/include/Buffer.h"
#include "net/include/InetAddress.h"

namespace CppUtil {
namespace Net {

class Socket;
class Channel;
class EventLoop;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;

class TcpConnection : public Noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                const InetAddress& localAddr, const InetAddress& peerAddr);
  ~TcpConnection();

  EventLoop* getLoop() const { return loop_; }
  const std::string& name() const { return name_; }
  void setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }

  void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
  void connectEstablished();
  void connectDestroyed();
  void send(Buffer* buf);
  void sendInLoop(const void* data, size_t len);
  void sendInLoop(const StringPiece& message);
 private:
  void handleRead();
  void handleWrite();
  void handleClose();

 private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
  EventLoop* loop_{nullptr};
  const std::string name_;
  StateE state_;  // FIXME: use atomic variable
  bool reading_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  CloseCallback closeCallback_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  std::any context_;
  Buffer inputBuffer_;
  Buffer outputBuffer_;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

}  // namespace Net
}  // namespace CppUtil