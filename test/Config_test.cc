//
// Created by gilee on 2022/05/02.
//
#include <gtest/gtest.h>
#include "../config/Config.hpp"

TEST(Config_test, readFileTest) {
    EXPECT_NO_THROW({
        try {
            Config e("../test.conf");
            e.runParse();
            std::cout << e << std::endl;
        } catch (std::exception const & e) {
            throw;
        }
        });
}