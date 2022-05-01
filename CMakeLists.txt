cmake_minimum_required(VERSION 3.21)
project(untitled1)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
add_executable(untitled1 main.cpp SocketController.cpp SocketController.hpp HTTP.cpp HTTP.hpp)