#include "net/include/TcpServer.h"
#include "common/include/Thread.h"
#include "net/include/EventLoop.h"
#include "net/include/Buffer.h"
#include "net/include/InetAddress.h"
#include "net/include/TcpConnection.h"
#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace CppUtil;
using namespace CppUtil::Net;
using namespace std::placeholders;
int numThreads = 0;

class EchoServer
{
 public:
  EchoServer(EventLoop* loop, const InetAddress& listenAddr)
    : server_(loop, listenAddr, "EchoServer"),
      oldCounter_(0)
  {
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, _1, _2));
    server_.setThreadNum(numThreads);
    // loop->runEvery(3.0, std::bind(&EchoServer::printThroughput, this));
  }

  void start()
  {
    // LOG_INFO << "starting " << numThreads << " threads.";
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    // LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
    //     << conn->localAddress().toIpPort() << " is "
    //     << (conn->connected() ? "UP" : "DOWN");
    // conn->setTcpNoDelay(true);
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf)
  {
    size_t len = buf->readableBytes();
    transferred_.fetch_add(len);
    receivedMessages_.fetch_add(1);
    // conn->send(buf);
  }

  void printThroughput()
  {
    // Timestamp endTime = Timestamp::now();
    int64_t newCounter = transferred_.load();
    int64_t bytes = newCounter - oldCounter_;
    // int64_t msgs = receivedMessages_.compare_exchange_strong(0, );
    // double time = timeDifference(endTime, startTime_);
    // printf("%4.3f MiB/s %4.3f Ki Msgs/s %6.2f bytes per msg\n",
    //     static_cast<double>(bytes)/time/1024/1024,
    //     static_cast<double>(msgs)/time/1024,
    //     static_cast<double>(bytes)/static_cast<double>(msgs));

    oldCounter_ = newCounter;
    // startTime_ = endTime;
  }

  TcpServer server_;
  std::atomic<int64_t > transferred_;
  std::atomic<int64_t > receivedMessages_;
  int64_t oldCounter_;
};

int main(int argc, char* argv[])
{
//   LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  if (argc > 1)
  {
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  InetAddress listenAddr(2007);
  EchoServer server(&loop, listenAddr);

  server.start();

  loop.loop();
}

