#include "common/include/Noncopyable.h"

namespace CppUtil {

namespace Net {

class InetAddress;

class Socket : public Noncopyable {
 public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

 public:
  int getFd() const { return sockfd_; }

  void bind(const InetAddress& localaddr);
  void listen();
  int accept(InetAddress* peeraddr);

  /// Enable/disable SO_REUSEADDR
  ///
  void setReuseAddr(bool on);

  ///
  /// Enable/disable SO_REUSEPORT
  ///
  void setReusePort(bool on);

  ///
  void setKeepAlive(bool on);

 private:
  const int sockfd_;
};

}  // namespace Net

}  // namespace CppUtil