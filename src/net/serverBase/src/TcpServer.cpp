#include "net/serverBase/include/TcpServer.h"

#include <functional>

#include "common/include/Logger.h"
#include "net/serverBase/include/EventLoop.h"
#include "net/serverBase/include/EventLoopThreadPool.h"
#include "net/serverBase/include/TcpConnection.h"

namespace CppUtil {

namespace Net {

TcpServer::TcpServer(const InetAddress& listenAddr, const std::string& name,
                     Option option)
    : loop_(new EventLoop()),
      ipPort_(listenAddr.toIpPort()),
      name_(name),
      acceptor_(new Acceptor(loop_, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop_, name_)),
      nextConnId_(-1) {
  acceptor_->setNewConnectionCallback(
      [this](int sockfd, const InetAddress& addr) {
        this->newConnection(sockfd, addr);
      });
}

TcpServer::~TcpServer() {
  for (auto& item : connections_) {
    TcpConnectionPtr conn(item.second);
    item.second.reset();
    conn->getLoop()->runInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
  }
}

void TcpServer::setThreadNum(int numThreads) {
  assert(0 <= numThreads);
  threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
  threadPool_->start(threadInitCallback_);
  loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  loop_->loop();
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
  EventLoop* ioloop = threadPool_->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof(buf), "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;
  InetAddress localAddr(SockUtil::getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  TcpConnectionPtr conn(
      new TcpConnection(ioloop, connName, sockfd, localAddr, peerAddr));

  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  //   conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      [this](const TcpConnectionPtr& cb) { this->removeConnection(cb); });
  {
    std::unique_lock<std::mutex> lk(mtx_);
    connections_[connName] = conn;
  }

  ioloop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

/// Not thread safe, but in loop
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
  LOG_DEBUG("%s", "remove connection")

  {
    std::unique_lock<std::mutex> lk(mtx_);
    connections_.erase(conn->name());
  }

  auto loopPtr = conn->getLoop();
  loopPtr->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

int TcpServer::getConnectionNum() const {
  std::unique_lock<std::mutex> lk(mtx_);
  return connections_.size();
}

}  // namespace Net
}  // namespace CppUtil