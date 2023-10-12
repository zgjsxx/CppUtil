#include "net/include/TcpServer.h"

#include <functional>

#include "net/include/EventLoop.h"
#include "net/include/EventLoopThreadPool.h"
#include "net/include/TcpConnection.h"

namespace CppUtil {

namespace Net {

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr,
                     const std::string& name, Option option)
    : loop_(loop),
      ipPort_(listenAddr.toIpPort()),
      name_(name),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      nextConnId_(-1) {
  auto newConnection = [this](int sockfd, const InetAddress& addr) {
    this->newConnection(sockfd, addr);
  };
  acceptor_->setNewConnectionCallback(newConnection);
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
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  //   conn->setWriteCompleteCallback(writeCompleteCallback_);
  ioloop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

/// Not thread safe, but in loop
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
  size_t n = connections_.erase(conn->name());
  (void)n;
  auto loopPtr = conn->getLoop();
  loopPtr->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

}  // namespace Net
}  // namespace CppUtil