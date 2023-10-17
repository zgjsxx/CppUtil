cmake_minimum_required(VERSION 3.17)
project(CommonTest)

set(UT_SOURCES 
    Status_test.cpp  
    ThreadPool_test.cpp  
    Logger_test.cpp
    SystemInfo_test.cpp
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
    )

    
    add_test(NAME, ${name} COMMAND ${name})
endforeach()