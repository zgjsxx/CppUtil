#include <cstring>
#include "net/include/Socket.h"
#include "net/include/InetAddress.h"
#include "net/include/SocketUtil.h"

namespace CppUtil {

namespace Net {

Socket::~Socket()
{
  SockUtil::close(sockfd_);
}

void Socket::bind(const InetAddress& localaddr) {
  // int ret = ::bind(sockfd_)
  SockUtil::bind(sockfd_, localaddr.getSockAddr());
}


void Socket::listen()
{
  SockUtil::listen(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
  struct sockaddr_in6 addr;
  memset(&addr, 0 ,sizeof addr);
  int connfd = SockUtil::accept(sockfd_, &addr);
  if (connfd >= 0)
  {
    peeraddr->setSockAddrInet6(addr);
  }
  return connfd;
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
    // LOG_SYSERR << "SO_REUSEPORT failed.";
  }
#else
  if (on)
  {
    // LOG_ERROR << "SO_REUSEPORT is not supported.";
  }
#endif
}

}  // namespace Net
}  // namespace CppUtil