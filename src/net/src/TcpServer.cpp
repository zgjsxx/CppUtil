#include <functional>
#include "net/include/TcpServer.h"
#include "net/include/EventLoopThreadPool.h"
#include "net/include/EventLoop.h"

namespace CppUtil
{

namespace Net
{

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const std::string& name,
                     Option option)
    : loop_(loop),
      name_(name),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_))
{

}

void TcpServer::start()
{
    threadPool_->start(threadInitCallback_);
    loop_->runInLoop(
        std::bind(&Acceptor::listen, acceptor_.get()));
}

}  // namespace Net

} // namespace CppUtil