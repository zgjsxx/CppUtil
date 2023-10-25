#include "common/include/Logger.h"
#include "common/include/StringUtil.h"
#include "common/include/Hash.h"
#include "net/serverBase/include/websocket/WebSocketServer.h"
#include "net/serverBase/include/websocket/WebSocketParser.h"
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

struct WebSocketContext {
  enum State { handshake_start = 0, handshake_finish = 1 };

  State state_{handshake_start};
};

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

    std::shared_ptr<void> context = std::make_shared<WebSocketContext>();
    WebSocketContext* websosketContext = (WebSocketContext*)context.get();
    conn->setContext(context);
  }
}

void WebSocketServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
  size_t len = buf->readableBytes();
  LOG_DEBUG("length: %d, data: %.*s", (int)len, (int)len, buf->peek())
  std::shared_ptr<void> context = conn->getContext();
  WebSocketContext* websocketContext = (WebSocketContext*)context.get();
  if (websocketContext->state_ < WebSocketContext::handshake_finish) {
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

  std::shared_ptr<void> parser = conn->getParser();
  WebSocketParser* webSocketParser = (WebSocketParser*)parser.get();
  webSocketParser->parseMsg(buf->peek(), buf->readableBytes());
  if (webSocketParser->parseFinish()) {
    std::string body = webSocketParser->getBody();
    LOG_DEBUG("get msg %s", webSocketParser->getBody().c_str());
    onWebSocketRequest(conn, body);
  }
}

void WebSocketServer::onWebSocketRequest(const TcpConnectionPtr& conn,
                                         std::string& msg) {
  // int flag = WS_OP_TEXT | WS_FINAL_FRAME | WS_HAS_MASK;
  int flag = WS_OP_TEXT | WS_FINAL_FRAME;
  size_t frame_len =
      websocket_calc_frame_size((websocket_flags)flag, msg.size());
  char* frame = (char*)malloc(sizeof(char) * frame_len);
  const char mask[4] = {'a', 'b', 'c', 'd'};
  websocket_build_frame(frame, (websocket_flags)flag, mask, msg.c_str(),
                        msg.size());
  Buffer buf;
  buf.append(frame, frame_len);
  conn->send(&buf);
}

void WebSocketServer::onRequest(const TcpConnectionPtr& conn,
                                const HttpRequest& req) {
  std::string connection = req.getHeader("Connection");
  Common::strToLower(connection);
  bool close =
      connection == "close" ||
      (req.getVersion() == HttpRequest::kHttp10 && connection != "keep-alive");
  HttpResponse response(close);
  std::shared_ptr<void> context = conn->getContext();
  WebSocketContext* websocketContext = (WebSocketContext*)context.get();
  websocketContext->state_ = WebSocketContext::handshake_finish;
  response.setStatusCode((http_status)101);
  response.addHeader("Upgrade", "websocket");
  response.addHeader("Connection", "Upgrade");
  std::string SecWebSocketKey = req.getHeader("Sec-WebSocket-Key");

  SecWebSocketKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string SecWebSocketAccept = Common::sha1sum(SecWebSocketKey);
  SecWebSocketAccept =
      Common::base64_encode((const unsigned char*)SecWebSocketAccept.c_str(),
                            SecWebSocketAccept.size());
  response.addHeader("Sec-WebSocket-Accept", SecWebSocketAccept);

  Buffer buf;
  response.appendToBuffer(&buf);

  std::shared_ptr<void> parser = std::make_shared<WebSocketParser>();
  WebSocketParser* webSocketParser = (WebSocketParser*)parser.get();
  webSocketParser->init();
  conn->setParser(parser);

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
