#include "net/serverBase/include/http/HttpResponse.h"

#include <stdio.h>

#include "net/serverBase/include/Buffer.h"

using namespace CppUtil;
using namespace CppUtil::Net;

void HttpResponse::appendToBuffer(Buffer* output) const {
  char buf[32] = {0};
  snprintf(buf, sizeof buf, "HTTP/1.1 %d %s", statusCode_,
           http_status_str((http_status)statusCode_));
  output->append(buf);
  output->append("\r\n");

  if (closeConnection_) {
    output->append("Connection: close\r\n");
  } else {
    if (body_.size() != 0) {
      snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
      output->append(buf);
    }
    if (headers_.find("Connection") == headers_.end()) {
      output->append("Connection: Keep-Alive\r\n");
    }
  }

  for (const auto& header : headers_) {
    output->append(header.first);
    output->append(": ");
    output->append(header.second);
    output->append("\r\n");
  }

  output->append("\r\n");
  output->append(body_);
}
