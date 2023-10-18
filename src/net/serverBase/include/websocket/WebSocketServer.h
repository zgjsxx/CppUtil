#pragma once

#include <string>

#include "net/serverBase/include/TcpServer.h"
#include "net/serverBase/include/http/HttpParser.h"
using std::string;
namespace CppUtil {
namespace Net {

class HttpRequest;
class HttpResponse;

/// A simple embeddable HTTP server designed for report status of a program.
/// It is not a fully HTTP 1.1 compliant server, but provides minimum features
/// that can communicate with HttpClient and Web browser.
/// It is synchronous, just like Java Servlet.
class WebSocketServer : Noncopyable {
 public:
  using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

  WebSocketServer(const InetAddress& listenAddr, const string& name,
                  TcpServer::Option option = TcpServer::kNoReusePort);

  EventLoop* getLoop() const { return server_.getLoop(); }

  /// Not thread safe, callback be registered before calling start().
  void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }

  void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

  void start();
  void registerHttpApi(const std::string& api, HttpCallback callback);

 private:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn, Buffer* buf);
  void onRequest(const TcpConnectionPtr&, const HttpRequest&);
  TcpServer server_;
  HttpCallback httpCallback_;
  std::map<std::string, HttpCallback> httpApiMap_;
};

}  // namespace Net
}  // namespace CppUtil
