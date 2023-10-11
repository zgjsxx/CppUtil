#include <string>
#include <memory>
#include <atomic>
#include "net/include/Acceptor.h"
#include "common/include/Noncopyable.h"

namespace CppUtil
{

namespace Net
{

class InetAddress;
class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)> ;  

class TcpServer : public Noncopyable
{
public:
  enum Option
  {
    kNoReusePort,
    kReusePort,
  };
  using ThreadInitCallback = std::function<void(EventLoop*)>;

  TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const std::string& nameArg,
            Option option = kNoReusePort);

  ~TcpServer(){};
  void start();

private:
  EventLoop* loop_{nullptr};
  const std::string ipPort_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_;
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  ThreadInitCallback threadInitCallback_;
  std::atomic<int> started_;
  int nextConnId_;

};

}  // namespace Net

} // namespace CppUtil