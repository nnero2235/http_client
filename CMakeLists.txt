cmake_minimum_required(VERSION 3.3)
project(http_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES http.c http.h d_error.h test.c)
add_executable(http_client ${SOURCE_FILES})