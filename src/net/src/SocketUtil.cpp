#include <unistd.h>
#include "net/include/SocketUtil.h"

namespace CppUtil {

namespace Net {

int createNonblocking(sa_family_t family) {
  int sockfd =
      ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
    //
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

ssize_t read(int sockfd, void *buf, size_t count)
{
  return ::read(sockfd, buf, count);
}

ssize_t write(int sockfd, const void *buf, size_t count)
{
  return ::write(sockfd, buf, count);
}


}  // namespace Net
}  // namespace CppUtil