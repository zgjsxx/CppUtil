#ifndef CPPUTIL_NET_INETADDRESS_H
#define CPPUTIL_NET_INETADDRESS_H
#include <string>
#include <netinet/in.h>

namespace CppUtil
{
namespace Net
{

class InetAddress 
{
public:
  InetAddress() = default;
   InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

    const struct sockaddr* getSockAddr() const { 
        return reinterpret_cast<const sockaddr*>(&addr6_); 
    }

    sa_family_t family() const { return addr_.sin_family; }   
private:
  union
  {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

}
}

#endif