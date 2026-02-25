cmake_minimum_required(VERSION 3.10)
project(shared)

set(CMAKE_CXX_STANDARD 17)

add_library(shared src/utils.cpp)
target_include_directories(shared PUBLIC include)