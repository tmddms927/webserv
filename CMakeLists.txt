cmake_minimum_required(VERSION 3.21)
project(untitled1)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
add_executable(untitled1 main.cpp SocketController.cpp SocketController.hpp HTTP.cpp HTTP.hpp Config.cpp Config.hpp)


#######################################################################test

#Add the google test subdirectory
add_subdirectory(gtest)

#include googletest/include dir
include_directories(gtest/googletest/include)

#include the googlemock/include dir
include_directories(gtest/googlemock/include)

add_executable(config Config.cpp Config.hpp test/Config_test.cc)

#Link with GoogleTest
target_link_libraries(config gtest gtest_main)

#Link with GoogleMock
target_link_libraries(config gmock gmock_main)