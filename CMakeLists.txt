cmake_minimum_required(VERSION 3.0)
project(LogParser)

set(CMAKE_CXX_STANDARD 17)

add_executable(network network.cpp utils.cpp utils.hpp network.hpp)
add_executable(disk utils.cpp utils.hpp disk.cpp disk.hpp)
