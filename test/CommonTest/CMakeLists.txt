cmake_minimum_required(VERSION 3.17)
project(CommonTest)

set(SOURCES 
    Status_test.cpp    
)

string(REPLACE ".cpp" "" UNITTEST_BIN  "${SOURCES}")

foreach(name ${UNITTEST_BIN})
    add_executable(${name} ${name}.cpp)
    target_include_directories(
        ${name} 
        PUBLIC ${CMAKE_SOURCE_DIR}/src
        PUBLIC ${CMAKE_SOURCE_DIR}/third_party/catch2
    )

    target_link_libraries(
        ${name} 
        vcommon
    )
    target_compile_options(${name} PRIVATE -std=c++17)
    
    add_test(NAME, ${name} COMMAND ${name})
endforeach()