#define CATCH_CONFIG_MAIN

#include <thread>
#include "catch.hpp"
#include "common/include/ScopeExit.h"

using namespace CppUtil;
using namespace CppUtil::Common;
TEST_CASE("1", "ScopeExit") {
  int i = 3;
  {
    ScopeExit scope;
    scope.setExitFunc([&i]() { i = 4; });
  }
  REQUIRE(i == 4);
}