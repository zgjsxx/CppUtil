#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <functional>
#include "net/serverBase/include/Buffer.h"
namespace CppUtil {
namespace Net {

class EventLoop;
class InetAddress;
class TcpConnection;
class Channel;
class Buffer;
class EventLoopThread;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using NewConnectionCallback = std::function<void(int)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
class TcpClient {
 public:
  TcpClient(const std::string& nameArg);

  ~TcpClient();

  void connect(const InetAddress& serverAddr);
  void disconnect();
  void stop();

  void write(std::string msg);
  void newConnection(int sockfd);
  void removeConnection(const TcpConnectionPtr& conn);
  void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }

  void setwriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }

 private:
  EventLoop* loop_{nullptr};
  const std::string name_;
  std::mutex mtx_;
  int clientFd_{-1};
  TcpConnectionPtr connection_{nullptr};
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  EventLoopThread* thread_{nullptr};
  WriteCompleteCallback writeCompleteCallback_;
  bool connected_{false};
};

}  // namespace Net
}  // namespace CppUtil