#pragma once

#include <string>
#include <memory>
#include "net/protocol/selfDefine/SimpleProtoParser.h"
#include "net/serverBase/include/TcpServer.h"

namespace CppUtil {
namespace Net {

class SimpleProtoServer : Noncopyable {
 public:
  using SimpleProtoCallback =
      std::function<void(const SimpleProtoMsg& req, SimpleProtoMsg* resp)>;

  SimpleProtoServer(const InetAddress& listenAddr, const std::string& name,
                    TcpServer::Option option = TcpServer::kNoReusePort);

  EventLoop* getLoop() const { return server_.getLoop(); }

  /// Not thread safe, callback be registered before calling start().
  void setSimpleProtoCallback(const SimpleProtoCallback& cb) {
    simpleProtoCallback_ = cb;
  }

  void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

  void start();

 private:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn, Buffer* buf);
  void onRequest(const TcpConnectionPtr&, const SimpleProtoMsg& req);
  TcpServer server_;
  SimpleProtoCallback simpleProtoCallback_;
};

}  // namespace Net
}  // namespace CppUtil
