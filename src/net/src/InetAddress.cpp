#include "net/include/InetAddress.h"
#include "net/include/SocketUtil.h"
#include <strings.h>

namespace CppUtil
{

namespace Net
{    
    
InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6)
{
    if(ipv6 || ip.find(":") != std::string::npos){
        //ipv6 
        bzero(&addr6_, sizeof(addr6_));
        SockUtil::convertIpAddress(ip.c_str(), port, &addr6_);
    }
    else{
        //ipv4
        bzero(&addr_, sizeof(addr_));
        SockUtil::convertIpAddress(ip.c_str(), port, &addr_);
    }
}
} // namespace Net
} // namespace CppUtil
