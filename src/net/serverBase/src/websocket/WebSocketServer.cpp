#include "common/include/Logger.h"
#include "common/include/StringUtil.h"
#include "net/serverBase/include/websocket/WebSocketServer.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/http/HttpRequest.h"
#include "net/serverBase/include/http/HttpResponse.h"
using namespace CppUtil;
using namespace CppUtil::Net;

namespace CppUtil {
namespace Net {
namespace Detail {

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
  resp->setStatusCode(HTTP_STATUS_NOT_FOUND);
  resp->setCloseConnection(true);
}

}  // namespace Detail
}  // namespace Net
}  // namespace CppUtil

using namespace std::placeholders;
WebSocketServer::WebSocketServer(const InetAddress& listenAddr,
                                 const string& name, TcpServer::Option option)
    : server_(listenAddr, name, option),
      httpCallback_(Detail::defaultHttpCallback) {
  server_.setConnectionCallback(
      std::bind(&WebSocketServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&WebSocketServer::onMessage, this, _1, _2));
}

void WebSocketServer::start() { server_.start(); }

void WebSocketServer::onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    LOG_DEBUG("set Parser")
    std::shared_ptr<void> parser = std::make_shared<HttpParser>();
    HttpParser* httpParser = (HttpParser*)parser.get();
    httpParser->init(HTTP_BOTH);
    conn->setParser(parser);
  }
}

void WebSocketServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
  size_t len = buf->readableBytes();
  LOG_DEBUG("length: %d, data: %.*s", (int)len, (int)len, buf->peek())
  std::shared_ptr<void> parser = conn->getParser();
  HttpParser* httpParser = (HttpParser*)parser.get();
  httpParser->parseMsg(buf->peek(), buf->readableBytes());
  buf->retrieve(len);

  if (httpParser->parseFinish()) {
    LOG_DEBUG("http parser finish")
    HttpRequest req;
    httpParser->fillHttpRequest(req);
    LOG_DEBUG("host: %s method: %s, url = %s, http version: %u.%u",
              req.getHost().c_str(), req.getMethodStr().c_str(),
              req.getUrl().c_str(), req.getMajorVer(), req.getMinorVer())
    onRequest(conn, req);
    httpParser->reset();
  }
}

void WebSocketServer::onRequest(const TcpConnectionPtr& conn,
                                const HttpRequest& req) {
  std::string connection = req.getHeader("Connection");
  Common::strToLower(connection);
  bool close =
      connection == "close" ||
      (req.getVersion() == HttpRequest::kHttp10 && connection != "keep-alive");
  HttpResponse response(close);
  if (httpApiMap_.find(req.getUrl()) != httpApiMap_.end()) {
    httpCallback_ = httpApiMap_[req.getUrl()];
  }

  httpCallback_(req, &response);

  Buffer buf;
  response.appendToBuffer(&buf);

  conn->send(&buf);
  if (response.closeConnection()) {
    conn->shutdown();
  }
}

void WebSocketServer::registerHttpApi(const std::string& api,
                                      HttpCallback callback) {
  if (httpApiMap_.find(api) == httpApiMap_.end()) {
    httpApiMap_[api] = std::move(callback);
  }
}
