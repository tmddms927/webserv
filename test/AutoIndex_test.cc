//
// Created by gilee on 2022/05/02.
//
#include <gtest/gtest.h>
#include "../autoindex/AutoIndex.hpp"

TEST(Config_test, readFileTest) {
    Config config("../test.conf");
    AutoIndex index(config.getConfig());
}