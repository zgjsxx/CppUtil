cmake_minimum_required(VERSION 3.17)

project(
    vnet
    VERSION 1.0.0
    LANGUAGES CXX
)

set(NET_SOURCES 
    src/EventLoop.cpp
    src/EpollMonitor.cpp
    src/Channel.cpp
    src/InetAddress.cpp
    src/SocketUtil.cpp
    src/TcpServer.cpp
    src/Socket.cpp
    src/Acceptor.cpp
    src/EventLoopThread.cpp
    src/EventLoopThreadPool.cpp
    )


add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE ${NET_SOURCES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
# target_compile_options(${PROJECT_NAME} PRIVATE -DCMAKE_CXX_STANDARD=17 -fvisibility=hidden)
target_compile_options(${PROJECT_NAME} PRIVATE -DCMAKE_CXX_STANDARD=17 -fvisibility=default)
