#include "net/serverBase/include/TcpClient.h"
#include "net/serverBase/include/InetAddress.h"
#include "net/serverBase/include/EventLoop.h"
#include "net/serverBase/include/SocketUtil.h"
#include "net/serverBase/include/Channel.h"
#include "net/serverBase/include/TcpConnection.h"
#include "net/serverBase/include/Buffer.h"
#include "net/serverBase/include/EventLoopThread.h"

namespace CppUtil {
namespace Net {

void defaultThreadInitCallback(EventLoop*) {}

TcpClient::TcpClient(const std::string& name) : name_(name) {
  thread_ = new EventLoopThread(defaultThreadInitCallback, name);
  // loop_ refer to a stack variable
  loop_ = thread_->waitThreadStart();
}

TcpClient::~TcpClient() {
  thread_->stop();
  delete thread_;
  // Notice loop_ refer to a stack variable, so do not delete it
}

void TcpClient::connect(const InetAddress& serverAddr) {
  clientFd_ = SockUtil::createNonblocking(serverAddr.family());
  int ret = SockUtil::connect(clientFd_, serverAddr.getSockAddr());
  int saveErrno = (ret == 0) ? 0 : errno;
  newConnection(clientFd_);
  connected_ = true;
}

void TcpClient::disconnect() {
  if (connection_) {
    connection_->shutdown();
  }
  connected_ = false;
}

void TcpClient::write(std::string msg) {
  connection_->sendInLoop(msg.c_str(), msg.size());
}

void TcpClient::newConnection(int sockfd) {
  InetAddress peerAddr(SockUtil::getPeerAddr(sockfd));
  char buf[32];
  snprintf(buf, sizeof(buf), ":%s", peerAddr.toIpPort().c_str());
  std::string connName = name_ + buf;
  InetAddress localAddr(SockUtil::getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  TcpConnectionPtr conn(
      new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

  connection_ = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  //   conn->setCloseCallback(
  //       std::bind(&TcpClient::removeConnection, this, _1));  // FIXME: unsafe
  conn->connectEstablished();
}
}  // namespace Net
}  // namespace CppUtil