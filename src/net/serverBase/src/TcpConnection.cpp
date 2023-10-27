#include "net/serverBase/include/TcpConnection.h"

#include "common/include/Logger.h"
#include "net/serverBase/include/Channel.h"
#include "net/serverBase/include/EventLoop.h"
#include "net/serverBase/include/Socket.h"

namespace CppUtil {
namespace Net {

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name,
                             int sockfd, const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop),
      name_(name),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      state_(kConnecting),
      socket_(std::make_unique<Socket>(sockfd)),
      channel_(std::make_unique<Channel>(loop, sockfd)) {
  // TcpConnection is created by Acceptor
  // channel's callback is set by TcpConnection
  channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
  channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {
  // loop_ refer to a borrowed obj, it does not owned by TcpConnection, so it is
  // not needed to delete it
}

void TcpConnection::handleRead() {
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->getFd(), &savedErrno);
  if (n > 0) {
    // callback
    messageCallback_(shared_from_this(), &inputBuffer_);
  } else if (n == 0) {
    handleClose();
  } else {
    errno = savedErrno;
    handleError();
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
        if (writeCompleteCallback_) {
          loop_->queueInLoop(
              std::bind(writeCompleteCallback_, shared_from_this()));
        }
        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      }
    } else {
      //   LOG_SYSERR << "TcpConnection::handleWrite";
      //   if (state_ == kDisconnecting) {
      //     shutdownInLoop();
      //   }
    }
  }
}

void TcpConnection::shutdownInLoop() {
  if (!channel_->isWriting()) {
    // we are not writing
    socket_->shutdownWrite();
  }
}

void TcpConnection::handleClose() {
  LOG_DEBUG("%s", "connection close")
  // remove channel from epoll monitor list
  channel_->disableAll();
  TcpConnectionPtr guardThis(shared_from_this());
  connectionCallback_(guardThis);
  if (closeCallback_) {
    closeCallback_(guardThis);
  }
}

void TcpConnection::connectEstablished() {
  channel_->tie(shared_from_this());
  setState(kConnected);
  channel_->enableReading();

  // do some extra work from user set
  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  if (state_ == kConnected) {
    setState(kDisconnected);
    channel_->disableAll();

    connectionCallback_(shared_from_this());
  }
  channel_->remove();
}

void TcpConnection::sendInLoop(const StringPiece& message) {
  sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len) {
  //   loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (state_ == kDisconnected) {
    // LOG_WARN << "disconnected, give up writing";
    return;
  }
  // if no thing in output queue, try writing directly
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = SockUtil::write(channel_->getFd(), data, len);
    if (nwrote >= 0) {
      remaining = len - nwrote;
      if (remaining == 0 && writeCompleteCallback_) {
        loop_->queueInLoop(
            std::bind(writeCompleteCallback_, shared_from_this()));
      }
    } else {  // nwrote < 0
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        if (errno == EPIPE || errno == ECONNRESET)  // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if (!faultError && remaining > 0) {
    size_t oldLen = outputBuffer_.readableBytes();
    if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ &&
        highWaterMarkCallback_) {
      loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(),
                                   oldLen + remaining));
    }
    outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
    if (!channel_->isWriting()) {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::send(Buffer* buf) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      // if send function called thread is same with event loop thread,
      // run it directly
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    } else {
      // or push it into queue
      loop_->runInLoop([this, buf]() {
        sendInLoop(buf->peek(), buf->readableBytes());
        buf->retrieveAll();
      });
    }
  }
}

void TcpConnection::send(const StringPiece& message) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(message);
    } else {
      loop_->runInLoop(
          [this, message]() { sendInLoop(message.data(), message.size()); });
    }
  }
}

void TcpConnection::shutdown() {
  // FIXME: use compare and swap
  if (state_ == kConnected) {
    setState(kDisconnecting);
    // FIXME: shared_from_this()?
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::handleError() {
  int err = SockUtil::getSocketError(channel_->getFd());
}

}  // namespace Net
}  // namespace CppUtil