
#include "common/include/Logger.h"
#include "common/include/StringUtil.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/simpleproto/SimpleProtoServer.h"

using namespace CppUtil;
using namespace CppUtil::Net;

using namespace std::placeholders;

namespace CppUtil {
namespace Net {
namespace Detail {

void defaultWebSocketCallback(const SimpleProtoMsg& req, SimpleProtoMsg* resp) {
  std::string body = "not found";
  resp->body = body;
  resp->head.len = body.size();
}

}  // namespace Detail
}  // namespace Net
}  // namespace CppUtil

SimpleProtoServer::SimpleProtoServer(const InetAddress& listenAddr,
                                     const std::string& name,
                                     TcpServer::Option option)
    : server_(listenAddr, name, option),
      simpleProtoCallback_(Detail::defaultWebSocketCallback) {
  server_.setConnectionCallback(
      std::bind(&SimpleProtoServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&SimpleProtoServer::onMessage, this, _1, _2));
}

void SimpleProtoServer::start() { server_.start(); }

void SimpleProtoServer::onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    LOG_DEBUG("set Parser")
    std::shared_ptr<void> parser = std::make_shared<SimpleProtoParser>();
    SimpleProtoParser* simpleProtoParser = (SimpleProtoParser*)parser.get();
    simpleProtoParser->init();
    conn->setParser(parser);
  }
}

void SimpleProtoServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
  size_t len = buf->readableBytes();
  LOG_DEBUG("length: %d, data: %.*s", (int)len, (int)len, buf->peek())
  std::shared_ptr<void> parser = conn->getParser();
  SimpleProtoParser* simpleProtoParser = (SimpleProtoParser*)parser.get();
  simpleProtoParser->parseMsg((char*)buf->peek(), buf->readableBytes());
  buf->retrieve(len);

  if (simpleProtoParser->parseFinish()) {
    LOG_DEBUG("http parser finish")
    SimpleProtoMsg req;
    req = simpleProtoParser->getMsg();
    onRequest(conn, req);
    simpleProtoParser->clear();
  }
}

void SimpleProtoServer::onRequest(const TcpConnectionPtr& conn,
                                  const SimpleProtoMsg& req) {
  SimpleProtoMsg response;
  simpleProtoCallback_(req, &response);

  Buffer buf;
  int len = sizeof(response.head) + response.body.size();
  uint8_t* data = new uint8_t[len];
  SimpleProtoEncode(&response, data);
  buf.append(data, len);

  conn->send(&buf);
  conn->shutdown();
}
