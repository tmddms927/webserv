cmake_minimum_required(VERSION 3.21)
project(untitled1)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -Wall -Werror -Wextra")
file(GLOB SRC ./*.hpp ./*.cpp
        config/*.h config/*.cpp config/*.hpp
        Server/*.hpp Server/*.cpp
        Server/ContentType/*.cpp Server/ContentType/*.hpp
        Server/URIParser/*.cpp Server/URIParser/*.hpp
        HTTP/*.hpp HTTP/*.cpp
        autoindex/*.cpp autoindex/*.hpp
        CGIInterface/*.cpp CGIInterface/CGIInterface.hpp
        utils/*.hpp utils/*.cpp)
file(GLOB CONF_TEST_SRC config/*.h config/*.cpp config/*.hpp)
file(GLOB INDEX_TEST_SRC AutoIndex/*.hpp AutoIndex/*.cpp ${CONF_TEST_SRC})
add_executable(untitled1 ${SRC})



#Add the google test subdirectory
add_subdirectory(gtest)

#include googletest/include dir
include_directories(gtest/googletest/include)

#include the googlemock/include dir
include_directories(gtest/googlemock/include)

#######################################################################test_config
add_executable(config test/Config_test.cc ${CONF_TEST_SRC} )

#Link with GoogleTest
target_link_libraries(config gtest gtest_main)

#Link with GoogleMock
target_link_libraries(config gmock gmock_main)
#######################################################################test_config

#######################################################################test_index
add_executable(index test/AutoIndex_test.cc ${INDEX_TEST_SRC} )

#Link with GoogleTest
target_link_libraries(index gtest gtest_main)

#Link with GoogleMock
target_link_libraries(index gmock gmock_main)
#######################################################################test_index
#######################################################################test_server
#######################################################################test_server