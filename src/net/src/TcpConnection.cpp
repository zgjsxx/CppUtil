#include "net/include/TcpConnection.h"

#include "net/include/Channel.h"
#include "net/include/Socket.h"

namespace CppUtil {
namespace Net {

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name,
                             int sockfd, const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop), name_(name), localAddr_(localAddr), peerAddr_(peerAddr) {}

void TcpConnection::handleRead() {
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->getFd(), &savedErrno);
  if (n > 0) {
    // callback
    messageCallback_(shared_from_this(), &inputBuffer_);
  } else if (n == 0) {
    // handleClose();
  } else {
    errno = savedErrno;
    // handleEroor();
  }
}

void TcpConnection::handleWrite() {
  if (channel_->isWriting()) {
    ssize_t n = SockUtil::write(channel_->getFd(), outputBuffer_.peek(),
                                outputBuffer_.readableBytes());
    if (n > 0) {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
        channel_->disableWriting();
        // if (writeCompleteCallback_)
        // {
        //   loop_->queueInLoop(std::bind(writeCompleteCallback_,
        //   shared_from_this()));
        // }
        if (state_ == kDisconnecting) {
          //   shutdownInLoop();
        }
      }
    } else {
      //   LOG_SYSERR << "TcpConnection::handleWrite";
      // if (state_ == kDisconnecting)
      // {
      //   shutdownInLoop();
      // }
    }
  } else {
  }
}

void TcpConnection::handleClose() {
  channel_->disableAll();
  TcpConnectionPtr guardThis(shared_from_this());
  connectionCallback_(guardThis);
  closeCallback_(guardThis);
}

void TcpConnection::connectEstablished() {
  //   setState(kConnected);
  //   channel_->tie(shared_from_this());
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  //   if (state_ == kConnected)
  //   {
  //     setState(kDisconnected);
  //     channel_->disableAll();

  //     connectionCallback_(shared_from_this());
  //   }
  channel_->remove();
}

}  // namespace Net
}  // namespace CppUtil