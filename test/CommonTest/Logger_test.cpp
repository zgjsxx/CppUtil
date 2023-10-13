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