#pragma once

#include <any>
#include <memory>
#include <string>

#include "common/include/Noncopyable.h"
#include "net/serverBase/include/Buffer.h"
#include "net/serverBase/include/InetAddress.h"

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
using HighWaterMarkCallback =
    std::function<void(const TcpConnectionPtr&, size_t)>;

class TcpConnection : public Noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                const InetAddress& localAddr, const InetAddress& peerAddr);
  ~TcpConnection();
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
  EventLoop* getLoop() const { return loop_; }
  const std::string& name() const { return name_; }
  void setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }

  void shutdownInLoop();
  void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }
  void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
  void setHighWaterMarkCallback(const HighWaterMarkCallback& cb,
                                size_t highWaterMark) {
    highWaterMarkCallback_ = cb;
    highWaterMark_ = highWaterMark;
  }
  void connectEstablished();
  void connectDestroyed();
  void send(Buffer* buf);
  void send(const StringPiece& message);
  void sendInLoop(const void* data, size_t len);
  void sendInLoop(const StringPiece& message);
  void setState(StateE s) { state_ = s; }
  bool connected() const { return state_ == kConnected; }
  const InetAddress& localAddress() const { return localAddr_; }
  const InetAddress& peerAddress() const { return peerAddr_; }
  void handleError();
  std::shared_ptr<void> getParser() { return parser_; }
  void setParser(std::shared_ptr<void>& parser) { parser_ = parser; }
  void shutdown();

 private:
  void handleRead();
  void handleWrite();
  void handleClose();

 private:
  EventLoop* loop_{nullptr};
  const std::string name_;
  StateE state_;  // FIXME: use atomic variable
  bool reading_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  HighWaterMarkCallback highWaterMarkCallback_;
  CloseCallback closeCallback_;
  size_t highWaterMark_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  std::shared_ptr<void> parser_{
      nullptr};  // used for parse a certain protocol, such as http
  Buffer inputBuffer_;
  Buffer outputBuffer_;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

}  // namespace Net
}  // namespace CppUtil