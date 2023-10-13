// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include "net/src/http/HttpServer.h"
#include "net/include/TcpConnection.h"
#include "common/include/Logger.h"
#include "net/src/http/HttpContext.h"
#include "net/src/http/HttpRequest.h"
#include "net/src/http/HttpResponse.h"
#include "common/include/Logger.h"
using namespace CppUtil;
using namespace CppUtil::Net;

namespace CppUtil
{
namespace Net
{
namespace Detail
{

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
  resp->setStatusCode(HttpResponse::k404NotFound);
  resp->setStatusMessage("Not Found");
  resp->setCloseConnection(true);
}

}  // namespace detail
}  // namespace net
}  // namespace muduo

using namespace std::placeholders;
HttpServer::HttpServer(const InetAddress& listenAddr,
                       const string& name,
                       TcpServer::Option option)
  : server_(listenAddr, name, option),
    httpCallback_(Detail::defaultHttpCallback)
{
  server_.setConnectionCallback(
      std::bind(&HttpServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&HttpServer::onMessage, this, _1, _2));
}

void HttpServer::start()
{
  // LOG_WARN << "HttpServer[" << server_.name()
  //   << "] starts listening on " << server_.ipPort();
  server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    HttpContext c;
    conn->setContext(&c);
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf)
{
  HttpContext* context = (HttpContext*)(conn->getMutableContext());

  size_t len = buf->readableBytes();
  LOG_DEBUG("data: %.*s", (int)len, buf->peek())
  if (!context->parseRequest(buf))
  {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }

  if (context->gotAll())
  {
    onRequest(conn, context->request());
    context->reset();
  }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
  const string& connection = req.getHeader("Connection");
  bool close = connection == "close" ||
    (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
  HttpResponse response(close);
  httpCallback_(req, &response);
  Buffer buf;
  response.appendToBuffer(&buf);
  conn->send(&buf);
  if (response.closeConnection())
  {
    conn->shutdown();
  }
}

