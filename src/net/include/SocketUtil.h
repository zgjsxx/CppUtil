#ifndef CPPUTIL_NET_SOCKETUTIL_H
#define CPPUTIL_NET_SOCKETUTIL_H

#include <string>
#include <arpa/inet.h>

namespace CppUtil
{

namespace Net
{
namespace SockUtil
{


int createNonblocking(sa_family_t family);

void bind(int sockfd, const struct sockaddr* addr);

void listen(int sockfd);

void convertIpAddress(const std::string& ip, uint16_t port, struct sockaddr_in* addr);

void convertIpAddress(const std::string& ip, uint16_t port, struct sockaddr_in6* addr);

ssize_t read(int sockfd, void *buf, size_t count);

ssize_t write(int sockfd, const void *buf, size_t count);

} // namespace SockUtil
} // namespace Net
} // namespace CppUtil
#endif