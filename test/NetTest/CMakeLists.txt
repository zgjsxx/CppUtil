cmake_minimum_required(VERSION 3.17)
project(NetTest)

set(SOURCES 
    TcpEchoServer_test.cpp
    TcpEchoClient_test.cpp
    TcpWebSocketServer_test.cpp
    HttpServer_test.cpp
    Buffer_test.cpp
    HttpParser_test.cpp
    WebSocketServer_test.cpp
    SelfDefineProtocol_test.cpp
    SimpleProtoParser_test.cpp
)

string(REPLACE ".cpp" "" UNITTEST_BIN  "${SOURCES}")

foreach(name ${UNITTEST_BIN})
    add_executable(${name} ${name}.cpp)
    target_include_directories(
        ${name} 
        PUBLIC ${CMAKE_SOURCE_DIR}/src
        PUBLIC ${CMAKE_SOURCE_DIR}/third_party/catch2
        PUBLIC ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.5/include
    )

    target_link_libraries(
        ${name} 
        vcommon
        vnet
        ${JSONCPP_LIB}
    )
    target_compile_options(${name} PRIVATE -std=c++17)
    
    add_test(NAME, ${name} COMMAND ${name})
endforeach()