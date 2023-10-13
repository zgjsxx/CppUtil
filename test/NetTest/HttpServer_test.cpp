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
#include "net/include/Buffer.h"
#include "net/include/EventLoop.h"
#include "net/include/InetAddress.h"
#include "net/include/TcpConnection.h"
#include "net/include/TcpServer.h"
#include "net/src/http/HttpServer.h"
#include "protocol/websocket/websocketParser.h"

using namespace CppUtil;
using namespace CppUtil::Net;
int main()
{
  CppUtil::Common::initLog("HttpServer_test.log");
  LOG_DEBUG("%s", "init log")
  InetAddress listenAddr(2007);
  HttpServer server(listenAddr, "http");

  ThreadPool threadpool("default");
  threadpool.start(1);
  auto task = [&server]() {
    while (1) {
    //   LOG_DEBUG("current connection: %d", server.getConnectionNum())
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  };
  threadpool.postTask(task);

  server.start();   
}