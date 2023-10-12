#ifndef CPPUTIL_NET_SOCKETUTIL_H
#define CPPUTIL_NET_SOCKETUTIL_H

#include <arpa/inet.h>
#include <sys/uio.h>

#include <string>

namespace CppUtil {

namespace Net {
namespace SockUtil {

int createNonblocking(sa_family_t family);

void bind(int sockfd, const struct sockaddr* addr);

void listen(int sockfd);

void convertIpAddress(const std::string& ip, uint16_t port,
                      struct sockaddr_in* addr);

void convertIpAddress(const std::string& ip, uint16_t port,
                      struct sockaddr_in6* addr);

ssize_t read(int sockfd, void* buf, size_t count);

ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);

ssize_t write(int sockfd, const void* buf, size_t count);

struct sockaddr_in6 getLocalAddr(int sockfd);

void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
void toIp(char* buf, size_t size, const struct sockaddr* addr);

inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64); }

inline uint32_t hostToNetwork32(uint32_t host32) { return htobe32(host32); }

inline uint16_t hostToNetwork16(uint16_t host16) { return htobe16(host16); }

inline uint64_t networkToHost64(uint64_t net64) { return be64toh(net64); }

inline uint32_t networkToHost32(uint32_t net32) { return be32toh(net32); }

inline uint16_t networkToHost16(uint16_t net16) { return be16toh(net16); }

}  // namespace SockUtil
}  // namespace Net
}  // namespace CppUtil
#endif