cmake_minimum_required(VERSION 3.17)

project(CppUtil)

message(STATUS "CMAKE_OUTPUT_LIB_PATH = ${CMAKE_CURRENT_BINARY_DIR}/lib") 

# build_openssl.sh <openssl_path> <install_lib_path>
execute_process(COMMAND sh ${CMAKE_CURRENT_LIST_DIR}/build_openssl.sh ${CMAKE_CURRENT_LIST_DIR}/third_party/openssl_1.1.1w   ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_subdirectory(third_party/jsoncpp-1.9.5)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
