#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "net/serverBase/include/http/HttpParser.h"
#include "net/serverBase/include/http/HttpRequest.h"
#include "net/serverBase/include/Buffer.h"
#include "common/include/Logger.h"
using namespace CppUtil;
using namespace CppUtil::Net;

TEST_CASE("1", "HttpParser") {
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append("GET /text.html HTTP/1.1\r\n");
  buffer.append("Accept: */*\r\n");
  buffer.append("Accept-Language: zh-cn\r\n");
  buffer.append(
      "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;.NET "
      "CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)\r\n");
  buffer.append("Host: 127.0.0.1\r\n");
  buffer.append("Connection: Keep-Alive\r\n");
  buffer.append("\r\n");

  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getUrl().c_str(), "/text.html") == 0);
  REQUIRE(strcmp(req.getMethodStr().c_str(), "GET") == 0);
  REQUIRE(strcmp(req.getHeader("Host").c_str(), "127.0.0.1") == 0);
  std::string header = req.getHeader("Connection");
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "Keep-Alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Language").c_str(), "zh-cn") == 0);
  REQUIRE(
      strcmp(req.getHeader("User-Agent").c_str(),
             "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;.NET "
             "CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)") ==
      0);
}