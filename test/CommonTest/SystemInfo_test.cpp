#define CATCH_CONFIG_MAIN

#include "common/include/SystemInfo.h"

#include <iostream>
#include <thread>

#include "catch.hpp"

using namespace CppUtil;
using namespace CppUtil::Common;

TEST_CASE("1", "SystemInfo") { std::cout << getCpuNum() << std::endl; }