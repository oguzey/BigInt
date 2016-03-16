cmake_minimum_required(VERSION 3.2)
project(BigInt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES  ./src/BigInt.cpp main.cpp)
add_executable(BigInt ${SOURCE_FILES})