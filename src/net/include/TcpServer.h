#include <string>
#include <memory>
#include "net/include/Acceptor.h"
#include "common/include/Noncopyable.h"

namespace CppUtil
{

namespace Net
{

class InetAddress;
class EventLoop;
class Acceptor;

class TcpServer : public Noncopyable
{
public:
  enum Option
  {
    kNoReusePort,
    kReusePort,
  };

  TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const std::string& nameArg,
            Option option = kNoReusePort);

  ~TcpServer(){};

private:
  EventLoop* loop_{nullptr};
  const std::string ipPort_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_;


};

}  // namespace Net

} // namespace CppUtil