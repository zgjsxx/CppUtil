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
    src/CountDownLatch.cpp
    src/Logger.cpp
    src/SystemInfo.cpp
    src/StringUtil.cpp
    src/Hash.cpp
)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE ${COMMON_SOURCES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    PUBLIC ${CMAKE_SOURCE_DIR}/third_party/openssl_1.1.1w/include/
)

find_library(
    CRYPTO_LIB
    NAMES libcrypto.so
    PATHS ${CMAKE_BINARY_DIR}/lib
)

find_library(
    SSL_LIB
    NAMES libssl.so
    PATHS ${CMAKE_BINARY_DIR}/lib
)

target_link_libraries(
    ${PROJECT_NAME} 
    ${CRYPTO_LIB}
    ${SSL_LIB}
)


set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
# target_compile_options(${PROJECT_NAME} PRIVATE -DCMAKE_CXX_STANDARD=17 -fvisibility=hidden)
target_compile_options(${PROJECT_NAME} PRIVATE -DCMAKE_CXX_STANDARD=17 -fvisibility=default)
