#include "net/serverBase/include/http/HttpRequest.h"

#include <stdio.h>

#include "net/protocol/http/http_parser.h"
#include "net/serverBase/include/Buffer.h"
using namespace CppUtil;
using namespace CppUtil::Net;

std::string HttpRequest::getMethodStr() const {
  return http_method_str((http_method)method_);
}