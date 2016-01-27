cmake_minimum_required(VERSION 3.3)
project(ModelUpdate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lsqlite3")

set(SOURCE_FILES main.cpp bootstrap.cpp bootstrap.h)

add_executable(ModelUpdate ${SOURCE_FILES})