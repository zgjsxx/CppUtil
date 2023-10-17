#pragma once
#include <netinet/in.h>

#include <string>

namespace CppUtil {
namespace Net {

class InetAddress {
 public:
  // InetAddress() = default;
  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false,
                       bool ipv6 = false);
  InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

  const struct sockaddr* getSockAddr() const {
    return reinterpret_cast<const sockaddr*>(&addr6_);
  }

  explicit InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

  explicit InetAddress(const struct sockaddr_in6& addr) : addr6_(addr) {}
  void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }
  std::string toIp() const;
  std::string toIpPort() const;
  sa_family_t family() const { return addr_.sin_family; }

 private:
  union {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

}  // namespace Net
}  // namespace CppUtil
