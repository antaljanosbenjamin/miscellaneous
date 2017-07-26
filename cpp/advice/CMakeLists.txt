cmake_minimum_required(VERSION 3.5)
project(ericsson)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall")

set(SOURCE_FILES main.cpp Point.cpp Point.h Advice.cpp Advice.h)
add_executable(ericsson ${SOURCE_FILES})