#include "net/serverBase/include/InetAddress.h"

#include <strings.h>

#include <cstring>

#include "net/serverBase/include/SocketUtil.h"

namespace CppUtil {

namespace Net {

struct in6_addr in6addr_any;      /* :: */
struct in6_addr in6addr_loopback; /* ::1 */
const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

InetAddress::InetAddress(uint16_t portArg, bool loopbackOnly, bool ipv6) {
  //   static_assert(offsetof(InetAddress, addr6_) == 0, "addr6_ offset 0");
  //   static_assert(offsetof(InetAddress, addr_) == 0, "addr_ offset 0");
  if (ipv6) {
    memset(&addr6_, 0, sizeof(addr6_));
    addr6_.sin6_family = AF_INET6;
    in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
    addr6_.sin6_addr = ip;
    addr6_.sin6_port = SockUtil::hostToNetwork16(portArg);
  } else {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = SockUtil::hostToNetwork32(ip);
    addr_.sin_port = SockUtil::hostToNetwork16(portArg);
  }
}

InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6) {
  if (ipv6 || ip.find(":") != std::string::npos) {
    // ipv6
    bzero(&addr6_, sizeof(addr6_));
    SockUtil::convertIpAddress(ip.c_str(), port, &addr6_);
  } else {
    // ipv4
    bzero(&addr_, sizeof(addr_));
    SockUtil::convertIpAddress(ip.c_str(), port, &addr_);
  }
}

std::string InetAddress::toIpPort() const {
  char buf[64] = "";
  SockUtil::toIpPort(buf, sizeof buf, getSockAddr());
  return buf;
}

std::string InetAddress::toIp() const {
  char buf[64] = "";
  SockUtil::toIp(buf, sizeof buf, getSockAddr());
  return buf;
}

}  // namespace Net
}  // namespace CppUtil
