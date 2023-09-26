cmake_minimum_required(VERSION 3.17)

project(
    vcommon
    VERSION 1.0.0
    LANGUAGES CXX
)

set(COMMON_SOURCES 
    src/Status.cpp
    src/Thread.cpp
    src/ThreadPool.cpp
    src/ProcessInfo.cpp
)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE ${COMMON_SOURCES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
target_compile_options(${PROJECT_NAME} PRIVATE -DCMAKE_CXX_STANDARD=17 -fvisibility=hidden)
