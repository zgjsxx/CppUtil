#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "common/include/Logger.h"
#include "net/serverBase/include/http/HttpParser.h"
#include "net/serverBase/include/Buffer.h"
#include "net/serverBase/include/http/HttpRequest.h"
using namespace CppUtil;
using namespace CppUtil::Net;

TEST_CASE("1", "HttpParser") {
  // Parse http request with no body info
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
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "Keep-Alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Language").c_str(), "zh-cn") == 0);
  REQUIRE(
      strcmp(req.getHeader("User-Agent").c_str(),
             "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;.NET "
             "CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)") ==
      0);
}

TEST_CASE("2", "HttpParser") {
  // Parse http request with body info
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append("GET /demo HTTP/1.1\r\n");
  buffer.append("User-Agent: PostmanRuntime-ApipistRuntime/1.1.0\r\n");
  buffer.append("Cache-Control: no-cache\r\n");
  buffer.append("content-type: application/json\r\n");
  buffer.append("Accept: */*\r\n");
  buffer.append("Accept-Encoding: gzip, deflate, br\r\n");
  buffer.append("Host: 192.168.26.11:8080\r\n");
  buffer.append("Connection: keep-alive\r\n");
  buffer.append("content-Length: 14\r\n");
  buffer.append("\r\n");

  std::string body = "{\"name\":\"tom\"}";
  buffer.append(body);

  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == true);
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getUrl().c_str(), "/demo") == 0);
  REQUIRE(strcmp(req.getMethodStr().c_str(), "GET") == 0);
  REQUIRE(strcmp(req.getHeader("Host").c_str(), "192.168.26.11:8080") == 0);
  REQUIRE(strcmp(req.getHeader("Cache-Control").c_str(), "no-cache") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Length").c_str(), "14") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Type").c_str(), "application/json") ==
          0);
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "keep-alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Encoding").c_str(),
                 "gzip, deflate, br") == 0);
  REQUIRE(strcmp(req.getHeader("User-Agent").c_str(),
                 "PostmanRuntime-ApipistRuntime/1.1.0") == 0);
  REQUIRE(strcmp(req.getBody().c_str(), "{\"name\":\"tom\"}") == 0);
}

TEST_CASE("3", "HttpParser") {
  // Parse http request with body info
  // Parse header and then body
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append("GET /demo HTTP/1.1\r\n");
  buffer.append("User-Agent: PostmanRuntime-ApipistRuntime/1.1.0\r\n");
  buffer.append("Cache-Control: no-cache\r\n");
  buffer.append("content-type: application/json\r\n");
  buffer.append("Accept: */*\r\n");
  buffer.append("Accept-Encoding: gzip, deflate, br\r\n");
  buffer.append("Host: 192.168.26.11:8080\r\n");
  buffer.append("Connection: keep-alive\r\n");
  buffer.append("content-Length: 14\r\n");
  buffer.append("\r\n");

  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == false);
  buffer.retrieveAll();
  std::string body = "{\"name\":\"tom\"}";
  buffer.append(body);
  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == true);
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getUrl().c_str(), "/demo") == 0);
  REQUIRE(strcmp(req.getMethodStr().c_str(), "GET") == 0);
  REQUIRE(strcmp(req.getHeader("Host").c_str(), "192.168.26.11:8080") == 0);
  REQUIRE(strcmp(req.getHeader("Cache-Control").c_str(), "no-cache") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Length").c_str(), "14") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Type").c_str(), "application/json") ==
          0);
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "keep-alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Encoding").c_str(),
                 "gzip, deflate, br") == 0);
  REQUIRE(strcmp(req.getHeader("User-Agent").c_str(),
                 "PostmanRuntime-ApipistRuntime/1.1.0") == 0);
  REQUIRE(strcmp(req.getBody().c_str(), "{\"name\":\"tom\"}") == 0);
}

TEST_CASE("4", "HttpParser") {
  // Parse http request with chunked body
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append("GET /demo HTTP/1.1\r\n");
  buffer.append("User-Agent: PostmanRuntime-ApipistRuntime/1.1.0\r\n");
  buffer.append("Cache-Control: no-cache\r\n");
  buffer.append("content-type: application/json\r\n");
  buffer.append("Accept: */*\r\n");
  buffer.append("Accept-Encoding: gzip, deflate, br\r\n");
  buffer.append("Host: 192.168.26.11:8080\r\n");
  buffer.append("Connection: keep-alive\r\n");
  buffer.append("Transfer-Encoding: chunked\r\n");
  buffer.append("\r\n");
  // first chunk
  buffer.append("e\r\n{\"name\":\"tom\"}\r\n");  // 0xe = 14
  // last chunk
  buffer.append("0\r\n\r\n");

  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == true);
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getUrl().c_str(), "/demo") == 0);
  REQUIRE(strcmp(req.getMethodStr().c_str(), "GET") == 0);
  REQUIRE(strcmp(req.getHeader("Host").c_str(), "192.168.26.11:8080") == 0);
  REQUIRE(strcmp(req.getHeader("Cache-Control").c_str(), "no-cache") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Type").c_str(), "application/json") ==
          0);
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "keep-alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Encoding").c_str(),
                 "gzip, deflate, br") == 0);
  REQUIRE(strcmp(req.getHeader("User-Agent").c_str(),
                 "PostmanRuntime-ApipistRuntime/1.1.0") == 0);
  std::string chunkBody = req.getBody();
  LOG_DEBUG("chunkBody = %s", chunkBody.c_str())
  REQUIRE(strcmp(req.getBody().c_str(), "{\"name\":\"tom\"}") == 0);
}

TEST_CASE("5", "httpParser") {
  // Parse http request with chunked body1
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append(
      "POST /post_chunked_all_your_base HTTP/1.1\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "1e\r\nall your base are belong to us\r\n"
      "0\r\n"
      "\r\n");
  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == true);
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getBody().c_str(), "all your base are belong to us") == 0);
}

TEST_CASE("6", "HttpParser") {
  // Parse http request with chunked body
  CppUtil::Common::initLog("HttpParser_test.log");
  HttpParser httpParser;
  httpParser.init(HTTP_BOTH);
  Buffer buffer;
  buffer.append("GET /demo HTTP/1.1\r\n");
  buffer.append("User-Agent: PostmanRuntime-ApipistRuntime/1.1.0\r\n");
  buffer.append("Cache-Control: no-cache\r\n");
  buffer.append("content-type: application/json\r\n");
  buffer.append("Accept: */*\r\n");
  buffer.append("Accept-Encoding: gzip, deflate, br\r\n");
  buffer.append("Host: 192.168.26.11:8080\r\n");
  buffer.append("Connection: keep-alive\r\n");
  buffer.append("Transfer-Encoding: chunked\r\n");
  buffer.append("\r\n");
  // first chunk
  buffer.append("e\r\n{\"name\":\"tom\"}\r\n");  // 0xe = 14
  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == false);
  buffer.retrieveAll();

  buffer.append("1e\r\nall your base are belong to us\r\n");
  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == false);
  buffer.retrieveAll();
  // last chunk
  buffer.append("0\r\n\r\n");
  httpParser.parseMsg(buffer.peek(), buffer.readableBytes());
  REQUIRE(httpParser.parseFinish() == true);
  buffer.retrieveAll();
  HttpRequest req;
  httpParser.fillHttpRequest(req);
  REQUIRE(strcmp(req.getUrl().c_str(), "/demo") == 0);
  REQUIRE(strcmp(req.getMethodStr().c_str(), "GET") == 0);
  REQUIRE(strcmp(req.getHeader("Host").c_str(), "192.168.26.11:8080") == 0);
  REQUIRE(strcmp(req.getHeader("Cache-Control").c_str(), "no-cache") == 0);
  REQUIRE(strcmp(req.getHeader("Content-Type").c_str(), "application/json") ==
          0);
  REQUIRE(strcmp(req.getHeader("Connection").c_str(), "keep-alive") == 0);
  REQUIRE(strcmp(req.getHeader("Accept").c_str(), "*/*") == 0);
  REQUIRE(strcmp(req.getHeader("Accept-Encoding").c_str(),
                 "gzip, deflate, br") == 0);
  REQUIRE(strcmp(req.getHeader("User-Agent").c_str(),
                 "PostmanRuntime-ApipistRuntime/1.1.0") == 0);
  std::string chunkBody = req.getBody();
  LOG_DEBUG("chunkBody = %s", chunkBody.c_str())
  REQUIRE(strcmp(req.getBody().c_str(),
                 "{\"name\":\"tom\"}all your base are belong to us") == 0);
}