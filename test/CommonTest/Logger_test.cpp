#define CATCH_CONFIG_MAIN

#include <thread>
#include "catch.hpp"
#include "common/include/Logger.h"

using namespace CppUtil;
using namespace CppUtil::Common;
TEST_CASE("1", "Logger"){
    FileLogger fileLogger("test.log");
    fileLogger.log(LogLevelDebug, __FILE__, __FUNCTION__, __LINE__, "%s", "test");
}