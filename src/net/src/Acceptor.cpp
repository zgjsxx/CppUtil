#include "net/include/Acceptor.h"
#include "net/include/SocketUtil.h"
#include "net/include/InetAddress.h"

namespace CppUtil
{
namespace Net
{

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
    :loop_(loop),
    acceptSocket_(createNonblocking(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.getFd())
{
    
}

}
}