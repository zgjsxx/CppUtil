#include <string>
#include <iostream>
#include "net/serverBase/include/TcpClient.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/InetAddress.h"
#include "common/include/Logger.h"

using namespace CppUtil;
using namespace CppUtil::Net;

class TcpEchoClient {
 public:
  TcpEchoClient(std::string name) : client_(name) {
    client_.setMessageCallback(
        [this](const TcpConnectionPtr& conn, Buffer* buf) {
          this->onMessage(conn, buf);
        });

    client_.setConnectionCallback(
        [this](const TcpConnectionPtr& conn) { this->onConnection(conn); });

    client_.setwriteCompleteCallback(
        [this](const TcpConnectionPtr& conn) { this->onWriteFinish(conn); });
  };

  void connect(const InetAddress& serverAddr) { client_.connect(serverAddr); }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    size_t len = buf->readableBytes();
    LOG_DEBUG("length: %d, data: %.*s", (int)len, (int)len, buf->peek())
  };

  void onWriteFinish(const TcpConnectionPtr& conn) { LOG_DEBUG("write finish") }

  void write(std::string msg) { client_.write(msg); };

  void onConnection(const TcpConnectionPtr& conn){
      LOG_DEBUG("des server connected")};

 private:
  TcpClient client_;
};

int main() {
  CppUtil::Common::initLog("TcpEchoClient_test.log");
  TcpEchoClient client("echo_client");
  InetAddress destAddress("127.0.0.1", 2023);
  client.connect(destAddress);
  client.write("test");
  std::cin.get();
}
