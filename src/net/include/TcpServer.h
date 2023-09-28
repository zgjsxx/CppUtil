#include <string>
#include "common/include/Noncopyable.h"

namespace CppUtil
{

namespace Net
{

class InetAddress;
class EventLoop;

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
};

}  // namespace Net

} // namespace CppUtil