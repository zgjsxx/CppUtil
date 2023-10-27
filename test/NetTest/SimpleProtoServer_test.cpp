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
#include "net/serverBase/include/Buffer.h"
#include "net/serverBase/include/EventLoop.h"
#include "net/serverBase/include/InetAddress.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/TcpServer.h"
#include "net/serverBase/include/simpleproto/SimpleProtoServer.h"

using namespace CppUtil;
using namespace CppUtil::Net;

int main() {
  CppUtil::Common::initLog("SimpleProtoServer_test.log");
  LOG_DEBUG("%s", "init log")
  InetAddress listenAddr(8080);
  SimpleProtoServer server(listenAddr, "SimpleProtoServer");

  server.start();
}