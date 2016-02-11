cmake_minimum_required(VERSION 3.3)
project(ModelUpdate)

INCLUDE_DIRECTORIES(/usr/local/include /Users/francisco/Downloads/cpp/src)
LINK_DIRECTORIES("/usr/local/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -lsqlite3 -lgsl -lgslcblas")

set(SOURCE_FILES main.cpp bootstrap.cpp bootstrap.h ImpactFunction.cpp ImpactFunction.h DBManager.cpp DBManager.h Sample.cpp Sample.h update.h update.cpp update.h)

add_executable(ModelUpdate ${SOURCE_FILES})