#include "net/include/TcpConnection.h"

#include "common/include/Logger.h"
#include "net/include/Channel.h"
#include "net/include/EventLoop.h"
#include "net/include/Socket.h"

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
      channel_(new Channel(loop, sockfd)) {
  // TcpConnection is created by Acceptor
  // channel's callback is set by TcpConnection
  channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
  channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {}

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
  closeCallback_(guardThis);
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
    } else  // nwrote < 0
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        // LOG_SYSERR << "TcpConnection::sendInLoop";
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
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    } else {
      void (TcpConnection::*fp)(const StringPiece& message) =
          &TcpConnection::sendInLoop;
      loop_->runInLoop(std::bind(fp,
                                 this,  // FIXME
                                 buf->retrieveAllAsString()));
      // std::forward<string>(message)));
    }
  }
}

void TcpConnection::send(const StringPiece& message)
{
  if (state_ == kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message);
    }
    else
    {
      void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
      loop_->runInLoop(
          std::bind(fp,
                    this,     // FIXME
                    message.as_string()));
                    //std::forward<string>(message)));
    }
  }
}

void TcpConnection::shutdown()
{
  // FIXME: use compare and swap
  if (state_ == kConnected)
  {
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