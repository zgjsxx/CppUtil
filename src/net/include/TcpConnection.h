#pragma once

#include <string>
#include "common/include/Noncopyable.h"

namespace CppUtil
{
namespace Net
{

class TcpConnection : public Noncopyable
{
public:
    TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                cosnt InetAddress& peerAddr
    );

private:
    EventLoop* loop_{nullptr};
    std::string name;
};

}
}