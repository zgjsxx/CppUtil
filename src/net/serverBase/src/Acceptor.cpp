#include "net/serverBase/include/Acceptor.h"

#include <fcntl.h>
#include <unistd.h>

#include "net/serverBase/include/InetAddress.h"
#include "net/serverBase/include/SocketUtil.h"

namespace CppUtil {
namespace Net {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr,
                   bool reuseport)
    : loop_(loop),
      acceptSocket_(SockUtil::createNonblocking(listenAddr.family())),
      acceptChannel_(loop, acceptSocket_.getFd()),
      listening_(false),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reuseport);
  acceptSocket_.bind(listenAddr);
  acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
  acceptChannel_.disableAll();
  acceptChannel_.remove();
  ::close(idleFd_);
}

void Acceptor::listen() {
  listening_ = true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
  InetAddress peerAddr;
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd >= 0) {
    if (newConnCallback_) {
      newConnCallback_(connfd, peerAddr);
    } else {
      ::close(connfd);
    }
  } else {
    if (errno == EMFILE) {
      ::close(idleFd_);
      idleFd_ = ::accept(acceptSocket_.getFd(), NULL, NULL);
      ::close(idleFd_);
      idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}

}  // namespace Net
}  // namespace CppUtil