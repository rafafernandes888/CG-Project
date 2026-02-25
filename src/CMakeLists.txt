cmake_minimum_required(VERSION 3.10)
project(CG_SRC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adiciona os subprojetos
add_subdirectory(engine)
add_subdirectory(shared)
add_subdirectory(generator)
