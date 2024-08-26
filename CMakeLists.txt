cmake_minimum_required(VERSION 3.28)
project(kd-tree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(kd_tree main.cpp)
