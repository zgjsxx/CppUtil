cmake_minimum_required(VERSION 3.17)

project(
    vnet
    VERSION 1.0.0
    LANGUAGES CXX
)

set(NET_SOURCES 
    serverBase/src/EventLoop.cpp
    serverBase/src/EpollMonitor.cpp
    serverBase/src/Channel.cpp
    serverBase/src/InetAddress.cpp
    serverBase/src/SocketUtil.cpp
    serverBase/src/TcpServer.cpp
    serverBase/src/Socket.cpp
    serverBase/src/Acceptor.cpp
    serverBase/src/EventLoopThread.cpp
    serverBase/src/EventLoopThreadPool.cpp
    serverBase/src/Buffer.cpp
    serverBase/src/TcpConnection.cpp
    serverBase/src/http/HttpServer.cpp
    serverBase/src/http/HttpRequest.cpp
    serverBase/src/http/HttpResponse.cpp
    serverBase/src/http/HttpParser.cpp
    serverBase/src/websocket/WebSocketServer.cpp
    serverBase/src/TcpClient.cpp
    protocol/http/http_parser.c
    protocol/websocket/websocket_parser.c
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
