#include <string>
#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
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

    {
      std::unique_lock<std::mutex> lk(mtx_);
      getResp_ = true;
      msg_.clear();
      msg_.append(buf->peek(), buf->readableBytes());
    }

    buf->retrieve(buf->readableBytes());
    cond_.notify_one();
  };

  void onWriteFinish(const TcpConnectionPtr& conn) { LOG_DEBUG("write finish") }

  void write(const std::string& msg) { client_.write(msg); };

  void read(std::string& msg) {
    std::unique_lock<std::mutex> lk(mtx_);
    cond_.wait(lk, [this]() { return getResp_; });
    getResp_ = false;
    msg = std::move(msg_);
  }

  void onConnection(const TcpConnectionPtr& conn){
      LOG_DEBUG("des server connected")};

  void disConnected() { client_.disconnect(); }

 private:
  TcpClient client_;
  std::condition_variable cond_;
  std::mutex mtx_;
  std::string msg_;
  bool getResp_{false};
};

int main() {
  CppUtil::Common::initLog("TcpEchoClient_test.log");
  TcpEchoClient client("echo_client");
  InetAddress destAddress("127.0.0.1", 2023);
  client.connect(destAddress);
  client.write("test");
  std::string resp;
  client.read(resp);
  LOG_DEBUG("get resp %s", resp.c_str())
  client.write("Hello World");
  resp.clear();
  client.read(resp);
  LOG_DEBUG("get resp %s", resp.c_str())
  client.disConnected();
}
