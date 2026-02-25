cmake_minimum_required(VERSION 3.10)
project(generator)

set(CMAKE_CXX_STANDARD 17)

include_directories(../shared/include)
include_directories(include)

add_executable(generator src/main.cpp src/plane.cpp src/cone.cpp)
target_link_libraries(generator shared)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(generator PRIVATE -fpermissive -w)
endif()