#define CATCH_CONFIG_MAIN

#include "common/include/Logger.h"

#include <thread>

#include "catch.hpp"

using namespace CppUtil;
using namespace CppUtil::Common;
TEST_CASE("1", "Logger") {
  FileLogger fileLogger("test.log");
  fileLogger.log(LogLevelDebug, __FILE__, __FUNCTION__, __LINE__, "%s", "test");
}

TEST_CASE("2", "Logger") {
  initLog("test.log");
  LOG_DEBUG("%s", "test")
}

void println(const char* format, ...) __attribute__((format(printf, 1, 2)));

void println(const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  printf("\n");
  va_end(argptr);
}

TEST_CASE("3", "Logger") {
  initLog("test.log");
  std::string str = "test";
  LOG_DEBUG("%s", str.c_str())
}

TEST_CASE("4", "Logger") {
  int a = 90;
  std::string str = "test";
  println("hello %s", str.c_str());
}