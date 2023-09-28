#ifndef CPPUTIL_NET_SOCKETUTIL_H
#define CPPUTIL_NET_SOCKETUTIL_H

#include <string>
#include <arpa/inet.h>

namespace CppUtil
{

namespace Net
{

int createNonblocking(sa_family_t family);

void bind(int sockfd, const struct sockaddr* addr);

void listen(int sockfd);

void convertIpAddress(const std::string& ip, uint16_t port, struct sockaddr_in* addr);

void convertIpAddress(const std::string& ip, uint16_t port, struct sockaddr_in6* addr);

} // namespace Net
} // namespace CppUtil
#endif