// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include "net/serverBase/src/http/HttpServer.h"

#include "common/include/Logger.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/http/HttpRequest.h"
#include "net/serverBase/include/http/HttpResponse.h"
using namespace CppUtil;
using namespace CppUtil::Net;

namespace CppUtil {
namespace Net {
namespace Detail {

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
  resp->setStatusCode(HttpResponse::k404NotFound);
  resp->setStatusMessage("Not Found");
  resp->setCloseConnection(true);
}

}  // namespace Detail
}  // namespace Net
}  // namespace CppUtil

using namespace std::placeholders;
HttpServer::HttpServer(const InetAddress& listenAddr, const string& name,
                       TcpServer::Option option)
    : server_(listenAddr, name, option),
      httpCallback_(Detail::defaultHttpCallback) {
  server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
  server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2));
  httpParser_.init(HTTP_BOTH);
}

void HttpServer::start() { server_.start(); }

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    // HttpContext c;
    // conn->setContext(&c);
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
  size_t len = buf->readableBytes();
  LOG_DEBUG("length: %d, data: %.*s", (int)len, (int)len, buf->peek())

  httpParser_.parseMsg(buf->peek(), buf->readableBytes());
  buf->retrieve(len);

  if (httpParser_.parseFinish()) {
    LOG_DEBUG("http parser finish")
    HttpRequest req;
    httpParser_.fillHttpRequest(req);
    LOG_DEBUG("method: %s, url = %s", req.getMethodStr().c_str(),
              req.getUrl().c_str())
    onRequest(conn, req);
    httpParser_.reset();
  }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn,
                           const HttpRequest& req) {
  const string& connection = req.getHeader("Connection");
  bool close =
      connection == "close" ||
      (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
  HttpResponse response(close);
  httpCallback_ = httpApiMap_[req.getUrl()];
  httpCallback_(req, &response);
  Buffer buf;
  response.appendToBuffer(&buf);

  conn->send(&buf);
  if (response.closeConnection()) {
    conn->shutdown();
  }
}

void HttpServer::registerHttpApi(const std::string& api,
                                 HttpCallback callback) {
  if (httpApiMap_.find(api) == httpApiMap_.end()) {
    httpApiMap_[api] = std::move(callback);
  }
}
