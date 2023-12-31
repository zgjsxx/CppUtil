#include <netinet/tcp.h>
#include <unistd.h>
#include <cstring>
#include "net/serverBase/include/SocketUtil.h"
#define SOCKUTIL_ERR -1;
namespace CppUtil {

namespace Net {

namespace SockUtil {
int createNonblocking(sa_family_t family) {
  int sockfd =
      ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
    return SOCKUTIL_ERR;
  }
  return sockfd;
}

void convertIpAddress(const std::string& ip, uint16_t port,
                      struct sockaddr_in* addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  if (::inet_pton(AF_INET, ip.c_str(), &addr->sin_addr) <= 0) {
    // error handler
  }
}

void convertIpAddress(const std::string& ip, uint16_t port,
                      struct sockaddr_in6* addr) {
  addr->sin6_family = AF_INET6;
  addr->sin6_port = htons(port);
  if (::inet_pton(AF_INET6, ip.c_str(), &addr->sin6_addr) <= 0) {
    // error handler
  }
}

void bind(int sockfd, const struct sockaddr* addr) {
  int ret =
      ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  // error
}

void listen(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  // error
}
void close(int sockfd) {
  if (::close(sockfd) < 0) {
    // LOG_SYSERR << "sockets::close";
  }
}

int accept(int sockfd, struct sockaddr_in6* addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr_in6));
#if defined(NO_ACCEPT4)
  int connfd = ::accept(sockfd, reinterpret_cast<sockaddr*>(addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
  int connfd = ::accept4(sockfd, reinterpret_cast<sockaddr*>(addr), &addrlen,
                         SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif

  return connfd;
}

int connect(int sockfd, const struct sockaddr* addr) {
  return ::connect(sockfd, addr,
                   static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

ssize_t read(int sockfd, void* buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t write(int sockfd, const void* buf, size_t count) {
  return ::write(sockfd, buf, count);
}

ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

int setTcpNoDelay(int sockfd, bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval,
                         static_cast<socklen_t>(sizeof(optval)));
  return ret;
}

struct sockaddr_in6 getLocalAddr(int sockfd) {
  struct sockaddr_in6 localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&localaddr), &addrlen) <
      0) {
    // LOG_SYSERR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in6 getPeerAddr(int sockfd) {
  struct sockaddr_in6 peeraddr;
  memset(&peeraddr, 0, sizeof(peeraddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, reinterpret_cast<sockaddr*>(&peeraddr), &addrlen) <
      0) {
    // LOG_SYSERR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

void toIp(char* buf, size_t size, const struct sockaddr* addr) {
  if (addr->sa_family == AF_INET) {
    const struct sockaddr_in* addr4 =
        reinterpret_cast<const struct sockaddr_in*>(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
  } else if (addr->sa_family == AF_INET6) {
    const struct sockaddr_in6* addr6 =
        reinterpret_cast<const struct sockaddr_in6*>(addr);
    ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
  }
}

void toIpPort(char* buf, size_t size, const struct sockaddr* addr) {
  if (addr->sa_family == AF_INET6) {
    buf[0] = '[';
    toIp(buf + 1, size - 1, addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in6* addr6 =
        reinterpret_cast<const struct sockaddr_in6*>(addr);
    uint16_t port = networkToHost16(addr6->sin6_port);
    snprintf(buf + end, size - end, "]:%u", port);
    return;
  }
  toIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in* addr4 =
      reinterpret_cast<const struct sockaddr_in*>(addr);
  uint16_t port = networkToHost16(addr4->sin_port);
  snprintf(buf + end, size - end, ":%u", port);
}

int getSocketError(int sockfd) {
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

void shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
  }
}

}  // namespace SockUtil
}  // namespace Net
}  // namespace CppUtil