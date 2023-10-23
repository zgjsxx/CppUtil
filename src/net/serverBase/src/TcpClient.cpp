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

TcpClient::TcpClient(const std::string& name)
    : loop_(new EventLoop()), name_(name) {
  char buf[name_.size() + 32] = {0};
  snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), 0);
  thread_ = new EventLoopThread(defaultThreadInitCallback, buf);
  loop_ = thread_->waitThreadStart();
}

TcpClient::~TcpClient() {
  delete thread_;
  // Notice loop_ refer to a stack variable, so do not delete it
}

void TcpClient::connect(const InetAddress& serverAddr) {
  clientFd_ = SockUtil::createNonblocking(serverAddr.family());
  int ret = SockUtil::connect(clientFd_, serverAddr.getSockAddr());
  int saveErrno = (ret == 0) ? 0 : errno;
  newConnection(clientFd_);
}

void TcpClient::write(std::string msg) {
  buf_.append(msg.c_str(), msg.size());
  connection_->send(&buf_);
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
  //   conn->setWriteCompleteCallback(writeCompleteCallback_);
  //   conn->setCloseCallback(
  //       std::bind(&TcpClient::removeConnection, this, _1));  // FIXME: unsafe
  conn->connectEstablished();
}
}  // namespace Net
}  // namespace CppUtil