cmake_minimum_required(VERSION 3.17)
project(CommonTest)

set(UT_SOURCES 
    Status_test.cpp  
    ThreadPool_test.cpp  
    Logger_test.cpp
    SystemInfo_test.cpp
    Hash_test.cpp
    StringUtil_test.cpp
    ScopeExit_test.cpp
)

message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

set(DEPEND_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/src/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/ProcessInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/Status.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/SystemInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/Thread.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/ThreadPool.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/CountDownLatch.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/Hash.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/StringUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/common/src/ScopeExit.cpp
)

set(CMAKE_CXX_STANDARD 17)

string(REPLACE ".cpp" "" UNITTEST_BIN  "${UT_SOURCES}")

foreach(name ${UNITTEST_BIN})
    add_executable(${name} ${DEPEND_SOURCES} ${name}.cpp )
    target_include_directories(
        ${name} 
        PUBLIC ${CMAKE_SOURCE_DIR}/src
        PUBLIC ${CMAKE_SOURCE_DIR}/third_party/catch2
    )

    target_link_libraries(
        ${name}
        ${CRYPTO_LIB}
        ${SSL_LIB} 
    )
endforeach()

add_test(NAME Status_test COMMAND Status_test)
add_test(NAME ThreadPool_test COMMAND ThreadPool_test)
add_test(NAME Logger_test COMMAND Logger_test)
add_test(NAME SystemInfo_test COMMAND SystemInfo_test)
add_test(NAME Hash_test COMMAND Hash_test)
add_test(NAME StringUtil_test COMMAND StringUtil_test)
add_test(NAME ScopeExit_test COMMAND ScopeExit_test)