#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <utility>

#include "common/include/CountDownLatch.h"
#include "common/include/Logger.h"
#include "common/include/Thread.h"
#include "common/include/ThreadPool.h"
#include "net/protocol/websocket/websocket_parser.h"
#include "net/serverBase/include/Buffer.h"
#include "net/serverBase/include/EventLoop.h"
#include "net/serverBase/include/InetAddress.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/TcpServer.h"

using namespace CppUtil;
using namespace CppUtil::Common;
using namespace CppUtil::Net;
using namespace std::placeholders;
int numThreads = 8;

int websocket_frame_header(websocket_parser* parser) {
  // parser->data->opcode = parser->flags & WS_OP_MASK;  // gets opcode
  // parser->data->is_final = parser->flags & WS_FIN;    // checks is final
  // frame if (parser->length) {
  //   parser->data->body = malloc(
  //       parser->length);  // allocate memory for frame body, if body exists
  // }
  return 0;
}

int websocket_frame_body(websocket_parser* parser, const char* at,
                         size_t length) {
  // if (parser->flags & WS_HAS_MASK) {
  //   // if frame has mask, we have to copy and decode data via
  //   // websocket_parser_copy_masked function
  //   websocket_parser_decode((char*)(&parser->data[parser->offset], at,
  //   length,
  //                           parser);
  // } else {
  //   memcpy((char*)&parser->data[parser->offset], at, length);
  // }
  // LOG_DEBUG("Body: %.*s", (int)length, at)
  return 0;
}

int websocket_frame_end(websocket_parser* parser) {
  // my_app_push_frame(parser->data);  // use parsed frame
  return 0;
}

class WebSocketServer {
 public:
  WebSocketServer(const InetAddress& listenAddr)
      : server_(listenAddr, "EchoServer"), oldCounter_(0) {
    server_.setConnectionCallback(
        std::bind(&WebSocketServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&WebSocketServer::onMessage, this, _1, _2));
    server_.setThreadNum(numThreads);
    // loop->runEvery(3.0, std::bind(&EchoServer::printThroughput, this));

    // websocket parser init
    websocket_parser_settings_init(&settings);
    settings.on_frame_header = websocket_frame_header;
    settings.on_frame_body = websocket_frame_body;
    settings.on_frame_end = websocket_frame_end;
    parser = (websocket_parser*)malloc(sizeof(websocket_parser));
    websocket_parser_init(parser);
    parser->data = (void*)this;
  }
  ~WebSocketServer() { free(parser); }
  void start() {
    LOG_INFO("starting %d threads", numThreads);
    server_.start();
  }
  int getConnectionNum() const { return server_.getConnectionNum(); }

 private:
  void onConnection(const TcpConnectionPtr& conn) {
    LOG_DEBUG("%s -> %s is %s", conn->peerAddress().toIpPort().c_str(),
              conn->localAddress().toIpPort().c_str(),
              conn->connected() ? "UP" : "DOWN")
    // conn->setTcpNoDelay(true);
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    size_t len = buf->readableBytes();
    std::cout << "len = " << len << std::endl;
    LOG_DEBUG("data: %.*s", (int)len, buf->peek())
    transferred_.fetch_add(len);
    receivedMessages_.fetch_add(1);
    // websocket is first http to handshake, so first prepare http
    size_t nread =
        websocket_parser_execute(parser, &settings, buf->peek(), len);
    // if (nread != data_len) {
    //   // some callback return a value other than 0
    // }
    // conn->send(buf);
  }

  void printThroughput() {
    int64_t newCounter = transferred_.load();
    int64_t bytes = newCounter - oldCounter_;
    // int64_t msgs = receivedMessages_.compare_exchange_strong(0, );
    oldCounter_ = newCounter;
  }

  TcpServer server_;
  std::atomic<int64_t> transferred_;
  std::atomic<int64_t> receivedMessages_;
  int64_t oldCounter_;
  websocket_parser_settings settings;
  websocket_parser* parser{nullptr};
};

int main(int argc, char* argv[]) {
  if (argc > 1) {
    numThreads = atoi(argv[1]);
  }

  CppUtil::Common::initLog("EchoServer.log");
  LOG_DEBUG("%s", "init log")
  InetAddress listenAddr(2007);
  WebSocketServer server(listenAddr);

  ThreadPool threadpool("default");
  threadpool.start(1);
  auto task = [&server]() {
    while (1) {
      LOG_DEBUG("current connection: %d", server.getConnectionNum())
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  };
  threadpool.postTask(task);

  server.start();
}
