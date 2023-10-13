#pragma once
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "common/include/Noncopyable.h"
#include "net/include/Acceptor.h"

namespace CppUtil {

namespace Net {

class InetAddress;
class EventLoop;
class Acceptor;
class EventLoopThreadPool;
class Buffer;
class TcpConnection;

class TcpServer : public Noncopyable {
 public:
  enum Option {
    kNoReusePort,
    kReusePort,
  };
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using ThreadInitCallback = std::function<void(EventLoop*)>;
  using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
  using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
  using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
  using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

  TcpServer(const InetAddress& listenAddr, const std::string& name,
            Option option = kNoReusePort);

  ~TcpServer();

 public:
  const std::string& ipPort() const { return ipPort_; }
  const std::string& name() const { return name_; }
  EventLoop* getLoop() const { return loop_; }
  void setThreadNum(int numThreads);
  void setThreadInitCallback(const ThreadInitCallback& cb) {
    threadInitCallback_ = cb;
  }
  void setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }
  void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }

  std::shared_ptr<EventLoopThreadPool> threadPool() { return threadPool_; }
  void start();

  // used for statistics
  int getConnectionNum() const;

 private:
  /// Not thread safe, but in loop
  void newConnection(int sockfd, const InetAddress& peerAddr);
  /// Thread safe.
  void removeConnection(const TcpConnectionPtr& conn);
  /// Not thread safe, but in loop
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

 private:
  EventLoop* loop_{nullptr};
  const std::string ipPort_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_;
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  ThreadInitCallback threadInitCallback_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  std::atomic<int> started_{-1};
  int nextConnId_{-1};
  ConnectionMap connections_;
  mutable std::mutex mtx_;
};

}  // namespace Net

}  // namespace CppUtil